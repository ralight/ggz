// GGZ logo -- POV-Ray version
// 2/2003 Christian Perle

#include "colors.inc"

camera {
  location <0, 6, -8>*6
  direction 2.2*6*z
  look_at 0
}

background { color White }

light_source { <-6, 2, -3> color White }
light_source { <5, 2, -8> color White }
light_source { <1, 10, -1> color White }

// radius for round edges   (0 < Rrad < 1)
#declare Rrad = 0.2;

// scaling factor for image maps
#declare ImgScl = 1.7;

#declare Rcyl = cylinder {
  <-(1-Rrad), 0, 0>, <1-Rrad, 0, 0>, Rrad
  translate <0, 1-Rrad, -(1-Rrad)>
}

#declare Rbal = sphere { <1-Rrad, 1-Rrad, -(1-Rrad)>, Rrad }

#declare Rhor = union {
  object { Rcyl }
  object { Rbal }
}

#declare Rcut = box { <-(1-Rrad), -(1-Rrad), -1>, <1-Rrad, 1-Rrad, 1> }

#declare Rbox1 = union {
  union {  // box without edges
    object { Rcut pigment { image_map { png "192_192_tux.png" once interpolate 2 } translate -.5 scale ImgScl } }
    object { Rcut pigment { image_map { png "192_192_punkte.png" once interpolate 2 } translate -.5 scale ImgScl } rotate 90*x }
    object { Rcut pigment { image_map { png "128_128_piek.png" once interpolate 2 } translate -.5 rotate -90*z scale ImgScl } rotate <90, 0, 90> }
  }
  // add top round edges
  object { Rhor }
  object { Rhor rotate 90*y }
  object { Rhor rotate 180*y }
  object { Rhor rotate 270*y }
  // add side round edges
  object { Rcyl rotate 90*z }
  object { Rcyl rotate 90*z rotate 90*y }
  object { Rcyl rotate 90*z rotate 180*y }
  object { Rcyl rotate 90*z rotate 270*y }
  // add bottom round edges
  object { Rhor rotate <180, 0, 0> }
  object { Rhor rotate <180, 90, 0> }
  object { Rhor rotate <180, 180, 0> }
  object { Rhor rotate <180, 270, 0> }
  bounded_by { box { <-1.01, -1.01, -1.01>, <1.01, 1.01, 1.01> } }
  no_shadow
}

#declare Rbox2 = union {
  union {  // box without edges
    object { Rcut pigment { image_map { png "192_192_network.png" once interpolate 2 } translate -.5 scale ImgScl } }
    object { Rcut pigment { image_map { png "192_192_springer.png" once interpolate 2 } translate -.5 scale ImgScl } rotate 90*x }
    object { Rcut pigment { image_map { png "128_128_herz.png" once interpolate 2 } translate -.5 rotate -90*z scale ImgScl } rotate <90, 0, 90> }
  }
  // add top round edges
  object { Rhor }
  object { Rhor rotate 90*y }
  object { Rhor rotate 180*y }
  object { Rhor rotate 270*y }
  // add side round edges
  object { Rcyl rotate 90*z }
  object { Rcyl rotate 90*z rotate 90*y }
  object { Rcyl rotate 90*z rotate 180*y }
  object { Rcyl rotate 90*z rotate 270*y }
  // add bottom round edges
  object { Rhor rotate <180, 0, 0> }
  object { Rhor rotate <180, 90, 0> }
  object { Rhor rotate <180, 180, 0> }
  object { Rhor rotate <180, 270, 0> }
  bounded_by { box { <-1.01, -1.01, -1.01>, <1.01, 1.01, 1.01> } }
  no_shadow
}

// arrange boxes
//object { Rbox1 pigment { White } rotate -35*y translate -1.2*x }
//object { Rbox2 pigment { White } rotate -35*y translate 1.2*x }
object { Rbox1 pigment { White } rotate -35*y rotate 0*z translate -1.2*x }
object { Rbox2 pigment { White } rotate -35*y rotate 0*z translate 1.2*x }
