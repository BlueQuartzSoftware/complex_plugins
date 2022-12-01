# Porting SIMPL Filters

## Generate the Stub Files

Use the **ComplexFilterGen** module distributed in ***SIMPL DREAM3D***. This module will automatically update the LegacyUUIDMapping for this **Plugin**.

    ComplexFilterGen -c [C++ SIMPL Classname] -o [/Path/to/Complex/Plugin/Dir]

For example 
    ComplexFilterGen -c ConvertData -o /Users/mjackson/complex/src/Plugins/ComplexCore


This will generate the stub filters with the correct UUID mapping from the older `SIMPL` filter to the ported `complex` filter.

For some handy code snippets, see `complex/docs/Porting_Filters.md`.

