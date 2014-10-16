// Tiny Raytracer (C) Gabriel Gambetta 2013
// ----------------------------------------
//
//  Configuration and scene
//
// Size of the canvas. w is also reused as a "big constant" / "+infinity"
var w = 60;

// Sphere: radius, [cx,  cy,  cz], R,  G,  B, specular exponent, reflectiveness 
// R, G, B in [0, 9], reflectiveness in [0..9].
var spheres = [
  w, [ 0, -w, 0],  9, 0, 0,  100,  4,
  1, [ 0,  1, 4],  9, 9, 9,  100,  2,
];

// Ambient light.
var ambient_light = 2;

// Point lights: intensity, [x,  y,  z]
// Intensities should add to 10, including ambient.
var lights = [
  8, [2, 2, 0]
];

// -----------------------------------------------------------------------------

// Shorten some names.
var math = Math;
var sqrt = math.sqrt;
var max = math.max;

// Global variables.
var out_idx = 0;

// Closure doesn't rename vars unless they're declared with "var", which takes
// space. So most vars are 1-letter and global:
//
// C: sphere center
// L: light vector
// N: surface normal at intersection
// X: intersection point
// a: quadratic equation constant
// b: quadratic equation constant
// c: color channel
// d: quadratic equation discriminant
// e: loop variable
// f: candidate parameter t
// h: half-width of the canvas
// i: illumination
// j: (ray origin) - (sphere center) 
// k: <N, L> 
// l: light index in loop
// n: <N, N>
// q: sphere index in loop
// r: sphere radius
// s: closest intersection sphere index
// t: closest intersection t
// u: intensity of lights[l] 
// v: closest sphere found in loop
//
// The exceptions are vars that need to be initialized here (we still pay the
// "a=", so we pay a single "var" above, and use nice names) and some vars in 
// trace_ray, which is recursive, so some of it vars can't be global.

// Get to the raw pixel data.
var raw_data = Array(w * w)

// Dot product.
function dot(A, B) {
  return A[0]*B[0] + A[1]*B[1] + A[2]*B[2];
}


// Helper: A_minus_Bk(A, B, k)  =  A - B*k. Since it's used more with k < 0,
// using - here saves a couple of bytes later.
function A_minus_Bk (A, B, k) {
  return [A[0] - B[0]*k, A[1] - B[1]*k, A[2] - B[2]*k];
}


// Find nearest intersection of the ray from B in direction D with any sphere.
// "Interesting" parameter values must be in the range [t_min, t_max].
// Returns the index within spheres of the center of the hit sphere, 0 if none.
// The parameter value for the intersection is in the global variable t.
function closest_intersection(B, D, t_min, t_max) {
  t = w;  // Min distance found.
  // Quadratic equation coefficients are K1, K2, K3. K1 is constant for the ray.
  a = 2*dot(D, D);  // 2*K1

  // For each sphere.
  // Get the radius and test for end of array at the same time; 
  // spheres[n] == undefined ends the loop.
  // q points to the 2nd element of the sphere because of q++; +6 skips to next
  // sphere.
  for (v = q = 0; r = spheres[q++]; q += 6) {  
    b = -2*dot(j = A_minus_Bk(B, spheres[q], 1), D);  // -K2; also j = origin - center
    
    // Compute sqrt(Discriminant) = sqrt(K2*K2 - 4*K1*K3), go ahead if there are
    // solutions.
    if ( d = sqrt(b*b - 2*a*(dot(j, j) - r*r)) ) {
      // Compute the two solutions.
      for (e = 2; e--; d = -d) {
        f = (b - d)/a;  // f = (-K2 - d) / 2*K1
        if (t_min < f && f < t_max && f < t) { 
          v = q;
          t = f;
        }
      }
    }
  }

  // Return index of closest sphere in range; t is global
  return v;
}


// Trace the ray from B with direction D considering hits in [t_min, t_max].
// If depth > 0, trace recursive reflection rays.
// Returns the value of the current color channel as "seen" through the ray.
function trace_ray(B, D, t_min, t_max, depth) {
  // Find nearest hit; if no hit, return black.
  if (!(s = closest_intersection(B, D, t_min, t_max)))
    return 0;
  
  // Compute "normal" at intersection: N = X - spheres[s]
  N = A_minus_Bk(X = A_minus_Bk(B, D, -t),  // intersection: X = B + D*t = B - D*(-t)
                 spheres[s], 1);

  // Instead of normalizing N, we divide by its length when appropriate. Most of
  // the time N appears twice, so we precompute its squared length.
  n = dot(N, N);

  // Start with ambient light only
  i = ambient_light;
  
  // For each light
  for (l = 0; u = lights[l++]; ) { // Get intensity and check for end of array

    // Compute vector from intersection to light (L = lights[l++] - X) and
    // k = <N,L> (reused below)
    k = dot(N, L = A_minus_Bk(lights[l++], X, 1));

    // Add to lighting
    i += u * 
      // If the pont isn't in shadow
      // [t_min, t_max]  =  [epsilon,  1] - epsilon avoids self-shadow, 1 
      // doesn't look farther than the light itself.
      !closest_intersection(X, L, 1/w, 1) * (
        // Diffuse lighting, only if it's facing the point 
        // <N,L> / (|N|*|L|) = cos(alpha)
        // Also, |N|*|L| = sqrt(<N,N>)*sqrt(<L,L>) = sqrt(<N,N>*<L,L>)
        max(0, k / sqrt(dot(L, L)*n))
      
        // Specular highlights
        //
        // specular = (<R,V>   / (|R|*|V|))   ^ exponent
        //          = (<-R,-V> / (|-R|*|-V|)) ^ exponent
        //          = (<-R,D>  / (|-R|*|D|))  ^ exponent
        //
        // R = 2*N*<N,L> - L
        // M = -R = -2*N*<N,L> + L = L + N*(-2*<N,L>)
        //
        // If the resultant intensity is negative, treat it as 0 (ignore it).
          + max(0, math.pow( dot(M = A_minus_Bk(L, N, 2*k/n), D) 
              / sqrt(dot(M, M)*dot(D, D)), spheres[s+4]))
      );
  }
  

  // Compute the color channel multiplied by the light intensity. 2.8 maps
  // the color range from [0, 9] to [0, 255] and the intensity from [0, 10]
  // to [0, 1],  because 2.8 ~ (255/9)/10
  // 
  // spheres[s] = sphere center, so spheres[s+c] = color channel
  // (c = [1..3] because ++c below)
  var local_color = spheres[s+c]*i*2.8;
  
  // If the recursion limit hasn't been hit yet, trace reflection rays.
  // N = normal (non-normalized - two divs by |N| = div by <N,N>
  // D = -view
  // R = 2*N*<N,V>/<N,N> - V = 2*N*<N,-D>/<N,N> + D = D - N*(2*<N,D>/<N,N>)
  var ref = spheres[s+5]/9;
  return depth-- ? trace_ray(X,
                             A_minus_Bk(D, N, 2*dot(N, D)/n),  // R
                             1/w, w, depth)*ref
                   + local_color*(1 - ref)
                 : local_color;
}

// For each y; also compute h=w/2 without paying an extra ";"
for (y = h=w/2; y-- > -h;) {

  // For each x
  for (x = -h; x++ < h;) {

    c=1; var R = trace_ray([0, 1, 0], [x/w, y/w, 1], 1, w, 2);
    c=2; var G = trace_ray([0, 1, 0], [x/w, y/w, 1], 1, w, 2);
    c=3; var B = trace_ray([0, 1, 0], [x/w, y/w, 1], 1, w, 2);

    raw_data[out_idx++] = (77 * R + 150 * G + 29 * B) >> 8;
  }
}

function Halftoning_ErrorDiffusionMethod(grayImage, height, width) {
    var tempImage = grayImage;
    var result = new Array(height * width);

    for ( var y = 0; y < height; y++ ) {
        for ( var x = 0; x < width; x++ ) {
            var f = tempImage[y * width + x];
            var e;

            if ( f > 127 ) {
                e = f - 255;
                f = 255;

            } else {
                e = f;
                f = 0;
            }

            result[y * width + x] = f;

            /* 誤差をばら撒く */
            if( x != width - 1 )
                tempImage[y * width + x + 1] += Math.floor((7 / 16) * e);

            if( (x != 0) && (y != height - 1) )
                tempImage[(y + 1) * width + x - 1] += Math.floor((3 / 16) * e);

            if( y != height - 1 )
                tempImage[(y + 1) * width + x] += Math.floor((5 / 16) * e);

            if (x != width - 1 && y != height - 1)
                tempImage[(y + 1) * width + x + 1] += Math.floor((1 / 16) * e);

        }
    }

    return result;
}

raw_data = Halftoning_ErrorDiffusionMethod(raw_data, w, w);

var canvas = document.getElementById("c");
canvas.width = canvas.height = w;

var context2d = canvas.getContext("2d");
var image_data = context2d.getImageData(0, 0, w, w);

for (var y=0; y<w; y++){
    for (var x=0; x<w; x++){
        var c = raw_data[y*w+x];

        image_data.data[y*w*4+x*4] = c;
        image_data.data[y*w*4+x*4+1] = c;
        image_data.data[y*w*4+x*4+2] = c;
        image_data.data[y*w*4+x*4+3] = 255;
    }
}

context2d.putImageData(image_data,0,0);

