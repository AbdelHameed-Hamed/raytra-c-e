#pragma once

#include "ray.c"

// Previously the structure of this project was simpler, you simply had a chain of files
// each one including the one previous to it like so:
//			  |A.c| => |B.c| => ... => |main.c|
// This worked up till a point so what I'm going to handle this is by diverging at one point
// and converging again like so:
//			
//										   /=> |1st.c| \
//  |A.c| => ... => |diverging_include.c| / ............\ => |converging_incl.c| =>...=>|main.c|
//										  \ ............/
//										   \=> |nth.c| /

typedef struct {
	Vec3 p;
	Vec3 normal;
	float t;
} IHit_Record, *Hit_Record;
