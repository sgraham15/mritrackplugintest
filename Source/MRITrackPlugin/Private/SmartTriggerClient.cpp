// Fill out your copyright notice in the Description page of Project Settings.
#include "SmartTriggerClient.h"
//#ifndef NETWORKING_API
//    #define NETWORKING_API
//#endif
#include "Runtime/Networking/Public/Networking.h"


// Sets default values
ASmartTriggerClient::ASmartTriggerClient()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASmartTriggerClient::BeginPlay()
{
	Super::BeginPlay();
	

    {
        FIPv4Address Addr;
        FIPv4Address::Parse( ServerAddress, Addr );

        //Create Socket

        auto Endpoint = FIPv4Endpoint( Addr, ServerPort );
        
        //BUFFER SIZE
        int32 BufferSize = 2 * 1024 * 1024;

        ListenSocket = FUdpSocketBuilder( "SmartTriggerClientSocket" )
            .AsNonBlocking()
            .AsReusable()
            .BoundToEndpoint( Endpoint )
            .WithReceiveBufferSize( BufferSize );

        FTimespan ThreadWaitTime = FTimespan::FromMilliseconds( 100 );
        UDPReceiver = new FUdpSocketReceiver( ListenSocket, ThreadWaitTime, TEXT( "SMC UDP Receiver" ) );
        UDPReceiver->OnDataReceived().BindUObject( this, &ASmartTriggerClient::Recv );

        UDPSender = new FUdpSocketSender( ListenSocket, TEXT( "SMC UDP Sender" ) );
    }

}

void ASmartTriggerClient::Recv( const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt )
{
    //ScreenMsg( "Received bytes", ArrayReaderPtr->Num() );

    //FAnyCustomData Data;
    //*ArrayReaderPtr << Data;		//Now de-serializing! See AnyCustomData.h

    

    //BP Event
    //BPEvent_DataReceived( Data );
    ProcessReceived( ArrayReaderPtr );

}

void ASmartTriggerClient::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
    Super::EndPlay( EndPlayReason );


    if( UDPReceiver )
    {
        delete UDPReceiver;
        UDPReceiver = nullptr;
    }

    if( UDPSender )
    {
        delete UDPSender;
        UDPSender = nullptr;
    }

    //Clear all sockets!
    //		makes sure repeat plays in Editor dont hold on to old sockets!
    if( ListenSocket )
    {
        ListenSocket->Close();
        ISocketSubsystem::Get( PLATFORM_SOCKETSUBSYSTEM )->DestroySocket( ListenSocket );
    }

}

// Called every frame
//void ASmartTriggerClient::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//}

bool ASmartTriggerClient::IdentifyServer()
{
    const FString sendBuffer( "identify" );
    
    TSharedRef<TArray<uint8>, ESPMode::ThreadSafe> data;

    FIPv4Address Addr;
    FIPv4Address::Parse( ServerAddress, Addr );
    FIPv4Endpoint recipient( Addr, ServerPort );

    const FString sendStr( "identify" );
    uint8 msgBytes[8];
    StringToBytes( sendStr, msgBytes, sizeof( msgBytes ) );
    data->Append( msgBytes, sizeof( msgBytes ) );

    IsServerIdentified = false;

    if( !UDPSender->Send( data, recipient ) )
    {
        UE_LOG( LogTemp, Warning, TEXT( "Failed to identify smart trigger at %s:%d\n" ), *ServerAddress, ServerPort );
    }
    else
    {
        IsServerIdentified = true;
    }

    OnIsServerIdentifiedChanged( IsServerIdentified );
    
    return IsServerIdentified;
}

bool ASmartTriggerClient::AddClientEndpoint( int address )
{
    if( ClientEndpointSet.Find( address ) == nullptr )
    {
        //auto strList = ClientEndpointSet.stringList();
        //strList.append( FString( ToStringAddress( address ) ) );
        //ClientEndpointSet.setStringList( strList );
        ClientEndpointSet.Add( address );

        return true;
    }

    return false;
}

void ASmartTriggerClient::ProcessWeaponInfo( const InfoPacket &pkt )
{
    if( pkt.type == 'N' )
    {
        int addr = ToIntAddress( pkt.address0, pkt.address1, pkt.address2 );
        AddClientEndpoint( addr );

        OnNewWeaponPacketReceived( addr, pkt.shotsLeft );
    }
    else if( pkt.type == 'I' )
    {
        int addr = ToIntAddress( pkt.address0, pkt.address1, pkt.address2 );
        AddClientEndpoint( addr );

        OnWeaponUpdatePacketReceived( addr, static_cast< WeaponMsg >( pkt.message ), pkt.shotsLeft );
    }
}

void ASmartTriggerClient::ProcessBatteryStatus( const BatteryStatusPacket &pkt )
{
    int addr = ToIntAddress( pkt.address0, pkt.address1, pkt.address2 );
    float voltage = ToVoltage( pkt.rawData0, pkt.rawData1 );
    AddClientEndpoint( addr );

    OnBatteryUpdatePacketReceived( addr, voltage );
}

void ASmartTriggerClient::ProcessImuAcceleration( const IMUAccPacket &pkt )
{
    float x = ToCoord( pkt.rawData0, pkt.rawData1 );
    float y = ToCoord( pkt.rawData2, pkt.rawData3 );
    float z = ToCoord( pkt.rawData4, pkt.rawData5 );
    int addr = ToIntAddress( pkt.address0, pkt.address1, pkt.address2 );
    AddClientEndpoint( addr );

    OnImuUpdatePacketReceived( addr, x, y, z );
}

float ASmartTriggerClient::ToCoord( uint8_t high, uint8_t low )
{
    float value = ( ( ( 255.0f * high ) + low ) / 65535.0f - 0.5f ) * 360.0f;
    return value;
}

float ASmartTriggerClient::ToVoltage( uint8_t high, uint8_t low )
{
    int rawVoltage = ( ( 255 * high ) + low );
    float voltage = rawVoltage / 58.4f;

    return voltage;
}

int ASmartTriggerClient::ToIntAddress( uint8_t b0, uint8_t b1, uint8_t b2 )
{
    int addr = ( b0 << 16 ) | ( b1 << 8 ) | b2;
    return addr;
}

int ASmartTriggerClient::ToIntAddress( const FString &addrStr )
{
    FString tokens[3];
    if( !addrStr.Split( ":", &tokens[0], &tokens[1] ) )
    {
        verify( 0 );
        return -1;
    }
    if( !tokens[1].Split( ":", &tokens[1], &tokens[2] ) )
    {
        verify( 0 );
        return -1;
    }

    uint8_t b0 = FParse::HexNumber( *tokens[0] );//strtol( tokens[0], nullptr, 16 );
    uint8_t b1 = FParse::HexNumber( *tokens[1] );//strtol( tokens[1], nullptr, 16 );
    uint8_t b2 = FParse::HexNumber( *tokens[2] );//strtol( tokens[2], nullptr, 16 );

    return ToIntAddress( b0, b1, b2 );
}

FString ASmartTriggerClient::ToStringAddress( int addrInt )
{
    uint8_t b0 = ( uint8_t ) ( ( addrInt >> 16 ) & 0xff );
    uint8_t b1 = ( uint8_t ) ( ( addrInt >> 8 ) & 0xff );
    uint8_t b2 = ( uint8_t ) ( ( addrInt ) & 0xff );

    FString result;
    char buffer[3];
#if _WIN32
    _itoa_s( b0, buffer, 16 );
    result += buffer;
    result += ':';
    _itoa_s( b1, buffer, 16 );
    result += buffer;
    result += ':';
    _itoa_s( b2, buffer, 16 );
    result += buffer;
    result += ':';
#else
    itoa( b0, buffer, 16 );
    result += buffer;
    result += ':';
    itoa( b1, buffer, 16 );
    result += buffer;
    result += ':';
    itoa( b2, buffer, 16 );
    result += buffer;
    result += ':';
#endif

    return  result;
}

uint8_t ASmartTriggerClient::VoltageToPercentage( float voltage )
{
    uint8_t percentage = ( uint8_t ) ( ( voltage - 9.5f ) * 100.0f / 3.4f ); // 0% = 9.4 V, 100% = 12.8 V, delta = 3.4 V

    if( voltage < 9.4 )
        percentage = 2; // just because 0% sounds bad
    else if( percentage > 100 )
        percentage = 100;

    return percentage;
}

void ASmartTriggerClient::ProcessReceived( const FArrayReaderPtr& ArrayReaderPtr )
{
    //Debug.Log( "receivedBytes.Length = " + receivedBytes.Length );
    //Process codes
    auto const *receivedBytes = ArrayReaderPtr->GetData();
    if( ArrayReaderPtr->Num() < 2 )
    {
        UE_LOG( LogTemp, Warning, TEXT( "ERROR: receivedBytes.Length < 2\n" ) );
        return;
    }

    auto size = receivedBytes[0];
    if( size == 0 )
    {
        UE_LOG( LogTemp, Warning, TEXT( "ERROR: message size is 0.\n" ) );
        return;
    }

    auto type = ( char ) receivedBytes[1];
    switch( type )
    {
    case 'A':
    {
        // IMU.
        auto const *imuPkt = reinterpret_cast< const IMUAccPacket * >( receivedBytes );
        float x = ToCoord( imuPkt->rawData0, imuPkt->rawData1 );
        float y = ToCoord( imuPkt->rawData2, imuPkt->rawData3 );
        float z = ToCoord( imuPkt->rawData4, imuPkt->rawData5 );
        //Debug.Log( String.Format( "IMU: {0:X}:{1:X}:{2:X} = ({3}, {4}, {5})", imuPkt->address0, imuPkt->address1, imuPkt->address2, x, y, z ) );
        ProcessImuAcceleration( *imuPkt );
        break;
    }
    case 'B':
    {
        // Battery.
        auto const *batteryPkt = reinterpret_cast< const BatteryStatusPacket * >( receivedBytes );
        float voltage = ToVoltage( batteryPkt->rawData0, batteryPkt->rawData1 );
        //Debug.Log( String.Format( "Battery: {0:X}:{1:X}:{2:X} = {3}", batteryPkt->address0, batteryPkt->address1, batteryPkt->address2, voltage ) );
        ProcessBatteryStatus( *batteryPkt );
        break;
    }
    case 'I':
    {
        // Weapon Info (button/trigger pressed).
        auto const *weaponInfoPkt = reinterpret_cast< const InfoPacket * >( receivedBytes );
        //Debug.Log( String.Format( "Info: {0:X}:{1:X}:{2:X} Shots Left: {3} Msg: {4}", weaponInfoPkt->address0, weaponInfoPkt->address1, weaponInfoPkt->address2, weaponInfoPkt->shotsLeft, ( ( WeaponMsg ) weaponInfoPkt->message ).ToString() ) );
        ProcessWeaponInfo( *weaponInfoPkt );
        break;
    }
    case 'N':
    {
        // New weapon.
        auto const *weaponInfoPkt = reinterpret_cast< const InfoPacket * >( receivedBytes );
        UE_LOG( LogTemp, Warning, TEXT( "New weapon: %03x:%03x:%03x\n "), weaponInfoPkt->address0, weaponInfoPkt->address1, weaponInfoPkt->address2 );
        ProcessWeaponInfo( *weaponInfoPkt );
        break;
    }
    default:
        UE_LOG( LogTemp, Warning, TEXT( "ERROR: Uknown smart trigger packet.\n" ) );
        break;
    }
}

//void ASmartTriggerClient::readPendingDatagrams()
//{
//    while( mUdpSocket.hasPendingDatagrams() )
//    {
//        QNetworkDatagram datagram = mUdpSocket.receiveDatagram();
//        ProcessReceived( datagram );
//    }
//}

void ASmartTriggerClient::OnNewWeaponPacketReceived_Implementation( int address, int shotsLeft )
{

}

void ASmartTriggerClient::OnWeaponUpdatePacketReceived_Implementation( int address, const WeaponMsg &msg, int shotsLeft )
{

}

void ASmartTriggerClient::OnBatteryUpdatePacketReceived_Implementation( int address, float voltage )
{

}

void ASmartTriggerClient::OnImuUpdatePacketReceived_Implementation( int address, float x, float y, float z )
{

}

void ASmartTriggerClient::OnIsServerIdentifiedChanged_Implementation( bool state )
{

}

void ASmartTriggerClient::OnServerAddressChanged_Implementation( const FString& addr )
{

}

void ASmartTriggerClient::OnServerPortChanged_Implementation( int port )
{

}

void ASmartTriggerClient::OnClientEndpointsChanged_Implementation()
{

}
