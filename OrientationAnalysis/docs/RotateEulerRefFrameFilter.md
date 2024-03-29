# RotateEulerRefFrame

## Group (Subgroup) ##

Orientation Analysis (Conversion)

## Description ##

This **Filter** performs a passive rotation (Right hand rule) of the Euler Angles about a user defined axis. The *reference frame* is being rotated and thus the *Euler Angles* necessary to represent the same orientation must change to account for the new *reference frame*.  The user can set an *angle* and an *axis* to define the rotation of the *reference frame*.

## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Rotation Axis-Angle | float (4x) | Axis-Angle in sample reference frame to rotate about. |

## Required Geometry ##

Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Element Data Array** | EulerAngles | float | (3) | Three angles defining the orientation of the **Cell** in Bunge convention (Z-X-Z) |

## Created Objects ##

None

## Example Pipelines ##

+ INL Export
+ Export Small IN100 ODF Data (StatsGenerator)
+ Edax IPF Colors
+ Confidence Index Histogram

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)

