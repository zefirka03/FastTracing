precision mediump float;

uniform sampler2D tx;
vec2 resolution = vec2(144, 90) * 5.;

#define SAMPLES 100  // HIGHER = NICER = SLOWER
#define DISTRIBUTION_BIAS 0.6 // between 0. and 1.
#define PIXEL_MULTIPLIER  1.5 // between 1. and 3. (keep low)
#define INVERSE_HUE_TOLERANCE 20.0 // (2. - 30.)

#define GOLDEN_ANGLE 2.3999632 //3PI-sqrt(5)PI

#define pow(a,b) pow(max(a,0.),b) // @morimea

mat2 sample2D = mat2(cos(GOLDEN_ANGLE),sin(GOLDEN_ANGLE),-sin(GOLDEN_ANGLE),cos(GOLDEN_ANGLE));

void sirBirdDenoise(out vec4 fragColor, in vec2 fragCoord, vec2 uv) {
    
    ivec2 crd = ivec2(fragCoord);    
    vec2 texsize = resolution;
    vec4 c = texture2D(tx, uv); 
    vec4 acc = c;
    float count = 1.0;
    float ra = c.a;
    int size = int(texsize.x)/8;
    for (int xoff = -1; xoff > (-size-1); xoff--) {
        float weight = 1.0-abs(float(xoff))/float(size);
        ivec2 xcrd = crd+ivec2(xoff,0);
        vec4 oth = texture2D(tx, vec2(xcrd));
        float ddist = abs(oth.a-ra);
        if (ddist<0.5) { 
            acc.rgb += oth.rgb*weight; 
            count += weight; 
            ra = oth.a; 
        }
    }
    ra = c.a;
    for (int xoff = 1; xoff < (size+1); xoff++) {
        float weight = 1.0-abs(float(xoff))/float(size);
        ivec2 xcrd = crd+ivec2(xoff,0);
        vec4 oth = texture2D(tx, vec2(xcrd));
        float ddist = abs(oth.a-ra);
        if (ddist<0.5) {
            acc.rgb += oth.rgb*weight; 
            count += weight; 
            ra = oth.a;
        }
    }
    
    // Is this an edge that needs some anti-aliasing?
    if (count<=1.0) {
        acc.rgb += 0.25*texture2D(tx, vec2(crd) + vec2(-2, 0)).rgb;
        acc.rgb += 0.5*texture2D(tx, vec2(crd) + vec2(-1, 0)).rgb;
        acc.rgb += 0.5*texture2D(tx, vec2(crd) + vec2(1, 0)).rgb;
        acc.rgb += 0.25*texture2D(tx, vec2(crd) + vec2(2, 0)).rgb;
        count += 1.5;
    }
    
    fragColor = acc * (1.0/count);
    fragColor.a = c.a;
    
}

void main(){
    vec2 uv = gl_FragCoord.xy/resolution;
    vec4 center = texture2D(tx, uv);
	
    vec4 col;

    sirBirdDenoise(col, gl_FragCoord, uv);

    gl_FragColor = col;
}