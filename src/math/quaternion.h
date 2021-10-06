#if !defined(QUATERNION_H)
#define QUATERNION_H

//
// Quaternion are always in radians and all further conversions are in radians too!
//

union Quaternion {
    f32 data[4];
    struct {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    };
};

#endif
