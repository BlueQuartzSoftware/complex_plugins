# Porting a Filter #

Depending on where the ported filter is coming from determines what you need to
do. Below we break up the two options currently available:

## Porting From ***SIMPL*** to ***Filters Folder*** ##

This will be the most common type of Filter porting. The steps for this are as
follows:

### Go to FreeNas and pull the custom build of DREAM3D ###
This custom build has **ALL** ***SIMPL*** plugins compiled so you don't need
to worry about what filters will be available

### Load up ***SIMPL*** DREAM3D and navigate to ***ComplexFilterGen*** ###
 Here you will need to set the command arguments using the following syntax:
> -c NameOfFilterToPort -o file/path/to/the/plugin/

Some nuances to note for this are as follows:
<ul>
   <li>The path to the plugin should be .../PluginName/ NOT .../PluginName/src/PluginName/</li>
   <li>The slash at the end of the filepath is necessary to work properly ie [.../PluginName/ NOT .../PluginName]</li>
   <li>The name of the filter should be the SIMPL name not what you want the complex name to be</li>
</ul>


## Porting stubs from existing folder to ***Filters Folder*** ##

Some plugins have existing stubs in folders other than the primary ***Filters***
folder.

### Move the Filter and Algorithm files to the active ***Filters Folder*** ###

### Update the Legacy UUID Maps ###
<ol>
  <li> Open the LegacyUUIDMapping header file for this Plugin </li>
  <li> Uncomment the include statement for the filter being moved </li>
  <li> Uncomment the map entry for the filter being moved </li>
</ol>
  
 When working with the ***LegacyUUIDMapping*** header file in this **Plugin** 
 be sure to make sure the commented out tokens are not removed. Their syntax is 
 one of the following:
 > ***@@__HEADER__TOKEN__DO__NOT__DELETE__@@***
 
 or
 
 > ***@@__MAP__UPDATE__TOKEN__DO__NOT__DELETE__@@***
 
 ### Update the CMakeLists.txt files to reflect the changes ###
 This includes the ones for the unit tests and the one at the plugin level
 