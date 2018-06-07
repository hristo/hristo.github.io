// parameters:
// width:  1 = standard 2x2 duplo width
// length: 1 = standard 2x2 duplo length
// height: 1 = minimal duplo height
 
duplo_box(2, 1, 2, 0.0);
duplo_complement(2, 1, 2);

// size definitions
ns = 8.4;  //nibble start offset
no = 6.53; //nibbleoffset
nbo = 16; // nibble bottom offset
duplowidth = 31.66;
duplolength = 31.66;
duploheight = 9.6;
duplowall = 1.55;

module duplo_complement(width, length, height) {
    // the cube minus the normal block
    difference() {
        translate([0, -length*duplolength-2*duplowall, -0.1])
            difference() {
                difference() {
                    cube([width*duplowidth,
                          length*duplolength,
                          height*duploheight], true);
                    // subtract the "shell" of the box, shave off some more
                    // to make sure the lid fits in
                    duplo_box(width, length, height, duplowall/1.0);
                }
                // remove most of the body
                translate([0, 0, height/10*duploheight])
                    cube([width*duplowidth+duplowall,
                          length*duplolength+duplowall,
                          height*duploheight-2*duplowall], true);
            };
        // shave off the bottom thin layer
        translate([0, -length*duplolength-2*duplowall, -height*duploheight])
            cube([width*duplowidth+duplowall,
                  length*duplolength+duplowall,
                  height*duploheight], true);
    }

	// add nibbles on bottom (rendered on top---part is meant to be flipped)
	for (j=[1:length]) {
		for (i=[1:width]) {
			translate([(i-1)*nbo,
                       (j-1)*nbo - length*duplolength-2*duplowall,
                       0 - height/4*duploheight])
                duplobottomnibble(height/3*duploheight);
			translate([(i-1)*-nbo,
                       (j-1)*-nbo - length*duplolength-2*duplowall,
                       0 - height/4*duploheight])
                duplobottomnibble(height/3*duploheight);
			translate([(i-1)*-nbo,
                       (j-1)*nbo - length*duplolength-2*duplowall,
                       0 - height/4*duploheight])
                duplobottomnibble(height/3*duploheight);
			translate([(i-1)*nbo,
                       (j-1)*-nbo - length*duplolength-2*duplowall,
                       0 - height/4*duploheight])
                duplobottomnibble(height/3*duploheight);
		}
	}
}

// allow an epsilon argument for a slightly bigger box, good for subtraction
module duplo_box(width, length, height, epsilon) {
	// the cube
    difference() {
        difference() {
            cube([width*duplowidth+epsilon,
                  length*duplolength+epsilon,
                  height*duploheight], true);
            translate([0,0,-duplowall]) 	
                cube([width*duplowidth-2*duplowall-epsilon,
                      length*duplolength-2*duplowall-epsilon,
                      height*duploheight], true);
        }
        union() {
            // nibbles on top
            for (j=[1:length]) {
                for (i=[1:width]) {
                    translate([i*ns+(i-1)*no,j*ns+(j-1)*no,
                               6.9+(height-1)*duploheight/2-duplowall])
                        duplonibble_hole();
                    translate([i*-ns+(i-1)*-no,j*ns+(j-1)*no,
                               6.9+(height-1)*duploheight/2-duplowall])
                        duplonibble_hole();
                    translate([i*ns+(i-1)*no,j*-ns+(j-1)*-no,
                               6.9+(height-1)*duploheight/2-duplowall])
                        duplonibble_hole();
                    translate([i*-ns+(i-1)*-no,j*-ns+(j-1)*-no,
                               6.9+(height-1)*duploheight/2-duplowall])
                        duplonibble_hole();
                }
            }
        }
	}

	// nibbles on top
	for (j=[1:length]) {
		for (i=[1:width]) {
			translate([i*ns+(i-1)*no,j*ns+(j-1)*no,
                       6.9+(height-1)*duploheight/2])
                duplonibble();
			translate([i*-ns+(i-1)*-no,j*ns+(j-1)*no,
                       6.9+(height-1)*duploheight/2])
                duplonibble();
			translate([i*ns+(i-1)*no,j*-ns+(j-1)*-no,
                       6.9+(height-1)*duploheight/2])
                duplonibble();
			translate([i*-ns+(i-1)*-no,j*-ns+(j-1)*-no,
                       6.9+(height-1)*duploheight/2])
                duplonibble();
		}
	}

	// little walls inside
	difference() {
		union() {
			for (j=[1:length]) {
				for (i=[1:width])	{
					translate([0,j*ns+(j-1)*no,0 ])
                        cube([width*duplowidth,1.35,height*duploheight],true);
					translate([0,j*-ns+(j-1)*-no,0 ])
                        cube([width*duplowidth,1.35,height*duploheight],true);
					translate([i*ns+(i-1)*no,0,0 ])
                        cube([1.35,length*duplolength,height*duploheight],true);
					translate([i*-ns+(i-1)*-no,0,0 ])
                        cube([1.35,length*duplolength,height*duploheight],true);
                    // wider up top to prevent lid from going too far
					translate([0,j*ns+(j-1)*no,height/4.0*duploheight])
                        cube([width*duplowidth,3.5,height/2.0*duploheight],true);
					translate([0,j*-ns+(j-1)*-no,height/4.0*duploheight])
                        cube([width*duplowidth,3.5,height/2.0*duploheight],true);
					translate([i*ns+(i-1)*no,0,height/4.0*duploheight])
                        cube([3.5,length*duplolength,height/2.0*duploheight],true);
					translate([i*-ns+(i-1)*-no,0,height/4.0*duploheight])
                        cube([3.5,length*duplolength,height/2.0*duploheight],true);
				}
			}
		}
		cube([width*duplowidth - 4*duplowall,
              length*duplolength-4*duplowall,
              height*duploheight+2], true);
	}
}

module duplonibble() {
	difference() {
		cylinder(r=4.7,h=4.5,center=true,$fs = 0.01);
		cylinder(r=3.4,h=5.5,center=true,$fs = 0.01);
	}
}

module duplonibble_hole() {
	cylinder(r=3.4,h=5.5,center=true,$fs = 0.01);
}

module duplobottomnibble(height) {
	difference() {
		cylinder(r=6.6,h=height,center=true,$fs = 0.01);
		cylinder(r=5.3,h=height+1,center=true,$fs = 0.01);
	}
}