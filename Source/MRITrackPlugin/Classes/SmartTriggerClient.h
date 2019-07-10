// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "Sockets.h"
//#include "Networking.h"
//#include "Networking/public/Common/UdpSocketReciever.h"
#include "UnrealString.h"
#include "ArrayReader.h"
#include "GameFramework/Actor.h"
//#include "Runtime/Networking/Public/Networking.h"
#include "SmartTriggerClient.generated.h"

class FSocket;
class FUdpSocketReceiver;
class FUdpSocketSender;
struct FIPv4Endpoint;

typedef TSharedPtr<FArrayReader, ESPMode::ThreadSafe> FArrayReaderPtr;


UENUM( BlueprintType )
enum class WeaponMsg : uint8
{
    Shot_Fired = 11,
    Trigger_GameOn = 12,
    Trigger_GameOff = 13,
    Trigger_PullOnSafe = 14,
    Trigger_NoAmmo = 15,
    // ChargeHandle / Pistol Slide
    ChargeHandle_ChamberedRound = 21,
    ChargeHandle_EjectRound = 22,
    ChargeHandle_EjectRound_NoMag = 23,
    ChargeHandle_BoltLocked = 24,
    ChargeHandle_BoltReleased = 25,
    Slide_LockBack = 26,
    Slide_Forward = 27,
    // Magazine:
    Magazine_Inserted = 31,
    Magazine_Ejected = 32,
    // Selector Switch:
    Selector_SafeEngaged = 41,
    Selector_HotSemi = 42,
    Selector_HotBurst = 43,
    // Auxiliary:
    Flashlight_On = 51,
    Flashlight_Off = 52,
    Laser_On = 53,
    Laser_Off = 54,
    BoltRelease_Pressed = 55,
    BoltRelease_Released = 56,
    // Malfunctions:
    Malfunction_Cleared = 61,
    Malfunction_Dud = 62,
    Updated_MalfuctionRate = 63,
    // Misc
    PowerUp = 101,
    NewID = 102,
    Updated_RoundsInFullMagazine = 103,
    ThreeD_Packet = 104,
    Interrupt = 105,
    Sync_Button = 106,
    ButtonOne_Pressed = 111,         // Rifle front left button
    ButtonOne_Released = 112,
    ButtonTwo_Pressed = 113,         // Rifle front right button
    ButtonTwo_Released = 114,
    Trigger_Pressed = 115,           // Trigger button
    Trigger_Released = 116,
    Reload_Pressed = 117,          // Magazine (insert)
    Reload_Released = 118,
    Slide_Pressed = 119,             // Pistol slide catch
    Slide_Released = 120,
    MagCatch_Pressed = 121,          // Pistol magazine catch
    MagCatch_Released = 122,
};

UCLASS()
class MRITRACKPLUGIN_API ASmartTriggerClient : public AActor
{
	GENERATED_BODY()
	
public:	
    
#pragma pack(push, stc, 1)
    struct InfoPacket
    {
        uint8_t size{ 0 };      // every packet starts off with the size transmitted
        char type{ 0 };         // I = info, etc.
        uint8_t address0{ 0 };  // the 3 mac address uint8_ts (uint8_t 0)
        uint8_t address1{ 0 };  // the 3 mac address uint8_ts (uint8_t 1)
        uint8_t address2{ 0 };  // the 3 mac address uint8_ts (uint8_t 2)
        uint8_t swversion{ 0 }; // the software version of the weapon...  Arris left over
        uint8_t shotsLeft{ 0 }; // how many shots remain
        uint8_t message{ 0 };   // enumerated message
        //[MarshalAs(UnmanagedType.ByValArray, SizeConst = 1 * 50)]
        //uint8_t[,] optionaluint8_ts;
        //[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4 * 10)]
        ///public int[,] optionalInts;
    };

    struct IMUAccPacket
    {
        uint8_t size{ 0 };     // every packet starts off with the size transmitted
        char type{ 0 };        // I = info, O = Orientation, etc.
        uint8_t address0{ 0 }; // the 3 mac address uint8_ts (uint8_t 0)
        uint8_t address1{ 0 }; // the 3 mac address uint8_ts (uint8_t 1)
        uint8_t address2{ 0 }; // the 3 mac address uint8_ts (uint8_t 2)
        uint8_t rawData0{ 0 }; // raw data coming from the IMU; 16 bit data in High/Low uint8_t order, X, Y and Z
        uint8_t rawData1{ 0 }; // raw data coming from the IMU; 16 bit data in High/Low uint8_t order, X, Y and Z
        uint8_t rawData2{ 0 }; // raw data coming from the IMU; 16 bit data in High/Low uint8_t order, X, Y and Z
        uint8_t rawData3{ 0 }; // raw data coming from the IMU; 16 bit data in High/Low uint8_t order, X, Y and Z
        uint8_t rawData4{ 0 }; // raw data coming from the IMU; 16 bit data in High/Low uint8_t order, X, Y and Z
        uint8_t rawData5{ 0 }; // raw data coming from the IMU; 16 bit data in High/Low uint8_t order, X, Y and Z
    };

    struct BatteryStatusPacket
    {
        uint8_t size{ 0 };     // every packet starts off with the size transmitted
        char type{ 0 };        // I = info, O = Orientation, etc.
        uint8_t address0{ 0 }; // the 3 mac address uint8_ts (uint8_t 0)
        uint8_t address1{ 0 }; // the 3 mac address uint8_ts (uint8_t 1)
        uint8_t address2{ 0 }; // the 3 mac address uint8_ts (uint8_t 2)
        uint8_t rawData0{ 0 }; // raw data for battery; 16 bit data raw battery (0-1023), char for Hr:Min uptime
        uint8_t rawData1{ 0 }; // raw data for battery; 16 bit data raw battery (0-1023), char for Hr:Min uptime
        uint8_t rawData2{ 0 }; // raw data for battery; 16 bit data raw battery (0-1023), char for Hr:Min uptime
        uint8_t rawData3{ 0 }; // raw data for battery; 16 bit data raw battery (0-1023), char for Hr:Min uptime
    };
#pragma pack(pop, stc)


	// Sets default values for this actor's properties
	ASmartTriggerClient();

    //UFUNCTION( BlueprintImplementableEvent, Category = "MRI" )
    //UFUNCTION( BlueprintNativeEvent, Category = "MRI" )
    //void OnSomeEvent();

    UFUNCTION( BlueprintNativeEvent, Category = "MRI" )
    void OnNewWeaponPacketReceived( int address, int shotsLeft );

    UFUNCTION( BlueprintNativeEvent, Category = "MRI" )
    void OnWeaponUpdatePacketReceived( int address, const WeaponMsg &msg, int shotsLeft );

    UFUNCTION( BlueprintNativeEvent, Category = "MRI" )
    void OnBatteryUpdatePacketReceived( int address, float voltage );

    UFUNCTION( BlueprintNativeEvent, Category = "MRI" )
    void OnImuUpdatePacketReceived( int address, float x, float y, float z );

    UFUNCTION( BlueprintNativeEvent, Category = "MRI" )
    void OnIsServerIdentifiedChanged( bool state );

    UFUNCTION( BlueprintNativeEvent, Category = "MRI" )
    void OnServerAddressChanged( const FString& addr );

    UFUNCTION( BlueprintNativeEvent, Category = "MRI" )
    void OnServerPortChanged( int port );

    UFUNCTION( BlueprintNativeEvent, Category = "MRI" )
    void OnClientEndpointsChanged();

    // The address of the local network interface to use. 
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "MRI" )
    FString ServerAddress;

    // The address of the local network interface to use. 
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "MRI" )
    int ServerPort{ 6000 };


protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

    // Called whenever this actor is being removed from a level 
    void EndPlay( const EEndPlayReason::Type EndPlayReason ) override;

    void Recv( const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt );
    
    bool IdentifyServer();
    bool AddClientEndpoint( int address );
    void ProcessReceived( const FArrayReaderPtr& ArrayReaderPtr );
    void ProcessWeaponInfo( const InfoPacket &pkt );
    void ProcessBatteryStatus( const BatteryStatusPacket &pkt );
    void ProcessImuAcceleration( const IMUAccPacket &pkt );
    static float ToCoord( uint8_t high, uint8_t low );
    static float ToVoltage( uint8_t high, uint8_t low );
    static int ToIntAddress( uint8_t b0, uint8_t b1, uint8_t b2 );
    static int ToIntAddress( const FString &addrStr );
    static FString ToStringAddress( int addrInt );
    static uint8_t VoltageToPercentage( float voltage );

    FSocket *ListenSocket{ nullptr };
    FUdpSocketReceiver *UDPReceiver{ nullptr };
    FUdpSocketSender *UDPSender{ nullptr };
    bool IsServerIdentified{ false };
    TSet<int> ClientEndpointSet;
public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

};
