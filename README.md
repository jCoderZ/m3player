================================================================================

        _ __ ___ |___ / _ __ | | __ _ _   _  ___ _ __ 
       | '_ ` _ \  |_ \| '_ \| |/ _` | | | |/ _ \ '__|
       | | | | | |___) | |_) | | (_| | |_| |  __/ |   
       |_| |_| |_|____/| .__/|_|\__,_|\__, |\___|_|   
                       |_|            |___/           

================================================================================


Prerequisites
================================================================================
* GCC 4.x
* gtk-doc-tools (gtkdocize)



Compatibility with Controllers
================================================================================

* 2Player on Android does not recognize a MediaRenderer in Version 2, only
  Version 1 is found.
* UPnPlay works

Differences between MediaRendererV1 and MediaRendererV2
================================================================================

Changed StateVariables:
  i4 AbsoluteCounterPosition (V1)
  ui4 AbsoluteCounterPosition (V2)

New StateVariables:
  CurrentMediaCategory
  DRMState

New Actions:
  SetStateVariables
  GetStateVariables
  GetDRMState
  GetMediaInfo_Ext
