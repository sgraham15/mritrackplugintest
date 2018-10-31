

Steps to set up a character:

1) Browse in Content Brower, Right click in folder with character and select Animation->Animation Blue Print.  Select both Anim Instance at the top  and skeleton name at the bottom.
2) Drag the blue print into the scene. Add the MRI Skeleton Component setting the source skeleton name.
3) Double click animation blue print.  Right click animation graph and create a new optitrack skeleton.  Right click and create source skeleton asset and streaming client origin (capture volume).  connect graph nodes together.  Hit compile in top left corner.


Note: 

Currently, it is necessary to start server before UE4 client to get rpc handshake to work.

To see log, Window->Developer Tools->Output logs.


Adding BluePrints to plugins:

 1) Create a "Content" directory in the plugins directory.
 2) Copy the UAssests into the conten folder.
 3) Package the plugin.
 4) Go to the Content Browser in the editor, check the option "Show Plugin Content".

Todo:

