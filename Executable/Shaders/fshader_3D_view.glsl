#version 140
#extension GL_ARB_compatibility: enable

uniform vec3 eye_position;
uniform sampler3D tex;
uniform sampler3D tex_label;

in vec3 pixel_position;
uniform vec3 box_min;
uniform vec3 box_max;
uniform vec3 up;

uniform float sample;
uniform sampler1D color_table;
uniform vec4 background_color;
uniform float Udiffu;
uniform float Uambi;
uniform float Uspecu;
uniform float Ushin;
uniform float l_t;

uniform vec3 v_size;

uniform int phong_enable;
uniform float sampling_rate;
uniform vec2 window_size;
uniform int rendering_type;

uniform vec3 roi_center;
uniform int roi_size;
uniform float threshold;
uniform int doingSegmentationSetting;

uniform float xy_plane_pos;


void main()
{


    vec3 right=normalize(cross(-eye_position,up));

    vec3 view_eye_position=eye_position+right;
    vec3 view_right=normalize(cross(-view_eye_position,up));
	view_eye_position=view_eye_position+up;
	vec3 view_up=normalize(cross(view_right,-view_eye_position));
	
    vec3 t_pixel_position=view_eye_position*0.5+
                               view_right*pixel_position.x*window_size.x+
                               view_up*pixel_position.y*window_size.y;


//    vec3 direction=normalize(t_pixel_position-view_eye_position);
    vec3 direction=normalize(-view_eye_position);
    vec4 cur_color=vec4(0,0,0,0);
    float alpha=0;


    int cn=0;
    vec3 a[2];
    float k;
    vec3 point;
    if(cn<2){
            k=(box_min.x-t_pixel_position.x)/direction.x;
            point=t_pixel_position+k*direction;
            if(point.y>=box_min.y && point.y<=box_max.y
                    && point.z>=box_min.z && point.z<=box_max.z){
                            a[cn++]=point;
            }
    }
    if(cn<2){
            k=(box_max.x-t_pixel_position.x)/direction.x;
            point=t_pixel_position+k*direction;
            if(point.y>=box_min.y && point.y<=box_max.y
                    && point.z>=box_min.z && point.z<=box_max.z){
                            a[cn++]=point;
            }
    }
    if(cn<2){
            k=(box_min.y-t_pixel_position.y)/direction.y;
            point=t_pixel_position+k*direction;
            if(point.x>=box_min.x && point.x<=box_max.x
                     && point.z>=box_min.z && point.z<=box_max.z){
                             a[cn++]=point;
            }
    }
    if(cn<2){
            k=(box_max.y-t_pixel_position.y)/direction.y;
            point=t_pixel_position+k*direction;
            if(point.x>=box_min.x && point.x<=box_max.x
                     && point.z>=box_min.z && point.z<=box_max.z){
                             a[cn++]=point;
            }
    }
    if(cn<2){
            k=(box_min.z-t_pixel_position.z)/direction.z;
            point=t_pixel_position+k*direction;
            if(point.x>=box_min.x && point.x<=box_max.x
                    && point.y>=box_min.y && point.y<=box_max.y){
                             a[cn++]=point;
            }
    }
    if(cn<2){
            k=(box_max.z-t_pixel_position.z)/direction.z;
            point=t_pixel_position+k*direction;
            if(point.x>=box_min.x && point.x<=box_max.x
                     && point.y>=box_min.y && point.y<=box_max.y){
                             a[cn++]=point;
            }
    }
    if(cn==2) {
            if(length(a[0]-view_eye_position)>length(a[1]-view_eye_position)){
                    vec3 t=a[0];
                    a[0]=a[1];
                    a[1]=t;
            }

            int sampling_num=int(length(a[1]-a[0])*sample*sampling_rate);
            vec3 normalize_box=1.0/(box_max-box_min);
            vec3 dir=(a[1]-a[0])/sampling_num;
            vec3 cur_location=a[0];
            vec3 d_l=view_eye_position+view_up*2+view_right*2;
            bool plane_xy=true;
            bool plane_zy=true;
            bool plane_xz=true;

            for(int i=0;i<sampling_num;i++){
                    cur_location=cur_location+dir;


                    vec3 texture_location=(cur_location-box_min)*normalize_box;
                    int edge_count=0;
                    float label=texture3D(tex_label,texture_location).x;
                    if(label!=0){
                            vec4 t_color=texture1D(color_table,0.5+label*0.5);
                            if(t_color.w==1){
                                cur_color=t_color;
                                alpha=1;
                                break;
                            }
                            cur_color=cur_color+t_color*t_color.w*t_color.w*(1-alpha);
                            alpha=alpha+(1-alpha)*t_color.w*t_color.w;
                            if(alpha>0.9)break;
                    }

                    float value=texture3D(tex,texture_location).x;
                    vec4 t_color=texture1D(color_table,value*0.5);
                    float t_alpha=t_color.w;


                    if(phong_enable==1){
                            if(alpha<l_t && t_color.w>0.01){
                                    float right_color=texture3D(tex,texture_location+vec3(1.0/v_size.x,0,0)).x;
                                    float left_color=texture3D(tex,texture_location-vec3(1.0/v_size.x,0,0)).x;
                                    float up_color=texture3D(tex,texture_location+vec3(0,1.0/v_size.y,0)).x;
                                    float down_color=texture3D(tex,texture_location-vec3(0,1.0/v_size.y,0)).x;
                                    float front_color=texture3D(tex,texture_location-vec3(0,0,1.0/v_size.z)).x;
                                    float back_color=texture3D(tex,texture_location-vec3(0,0,1.0/v_size.z)).x;
                                    vec3 gr=-vec3(right_color-left_color,up_color-down_color,front_color-back_color);
                                    vec3 L=normalize(d_l-cur_location);
                                    vec3 E=normalize(view_eye_position-cur_location);
                                    vec3 R=normalize(-reflect(L,normalize(gr)));
                                    vec4 diffu=vec4(Udiffu,Udiffu,Udiffu,Udiffu)*max(dot(normalize(gr),L),0.0)*2;
                                    diffu=clamp(diffu,0.0,1.0);
//                                    vec4 specu=vec4(Uspecu,Uspecu,Uspecu,Uspecu)*pow(max(dot(R,E),0.0),Ushin*0.1);
//                                    specu=clamp(specu,0.0,1.0);
                                    t_color=t_color+Uambi+diffu;//+specu;
                            }
                    }
                    cur_color=cur_color+t_color*(1-alpha)/sampling_rate;
                    alpha=alpha+(1-alpha)*t_alpha/sampling_rate;
                    if(alpha>0.9)break;
            }
    }
    cur_color=cur_color+background_color*(1-alpha);
    gl_FragColor=cur_color;
}
