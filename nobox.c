#include <stdlib.h>
#include <sys/wait.h>
#include <xcb/xcb.h>
#ifdef COMPOSITE
#include <xcb/composite.h>
#endif
static const uint32_t di[]={0,0,1680,1050},cwa=XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT|XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY|XCB_EVENT_MASK_STRUCTURE_NOTIFY;
static uint32_t buf[7];
int main(int argc,char**argv){
	xcb_connection_t*d=xcb_connect(0,0);
	int32_t*x,*y,*tx=0,mx,my,rt=xcb_setup_roots_iterator(xcb_get_setup(d)).data->root,cs[255],*cz=cs+1;
	uint8_t mz,mZ;
	xcb_change_window_attributes(d,rt,XCB_CW_EVENT_MASK,&cwa);
	xcb_grab_key(d,1,rt,0,64,XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC);
	xcb_grab_key(d,1,rt,8,XCB_GRAB_ANY,XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC);
	xcb_grab_button(d,1,rt,XCB_EVENT_MASK_BUTTON_PRESS,XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC,XCB_NONE,XCB_NONE,XCB_GRAB_ANY,8);
	#ifdef COMPOSITE
	xcb_composite_redirect_subwindows(d,rt,XCB_COMPOSITE_REDIRECT_AUTOMATIC);
	#endif
	xcb_generic_event_t*e=0;
main:xcb_flush(d);
	waitpid(-1,0,WNOHANG);
noflush:x=y=cz-1;
again:free(e);
	switch((e=xcb_wait_for_event(d))->response_type&127){
	case XCB_BUTTON_PRESS:
		for(;x>cs;x--)
			if(*x==((xcb_button_press_event_t*)e)->child){
				if(((xcb_key_press_event_t*)e)->detail==2)goto pocus;
			case XCB_KEY_PRESS:
				mz=128|((xcb_key_press_event_t*)e)->detail;
				my=((xcb_key_press_event_t*)e)->state;
				goto*(cz==cs+1?&&kcode:&&stack);
			}
		goto noflush;
	case XCB_KEY_RELEASE:
		if(((xcb_key_press_event_t*)e)->detail!=64||!tx)default:goto again;
		xt:x=tx;
		tx=0;
		goto stack;
	case XCB_CONFIGURE_REQUEST:{
		void*p=buf;
		for(mz=0;mz<5;mz++)
			if(((xcb_configure_request_event_t*)e)->value_mask&1<<mz){*(uint32_t*)p=*(int16_t*)(((void*)e)+16+mz*2);p+=4;}
		if(((xcb_configure_request_event_t*)e)->value_mask&XCB_CONFIG_WINDOW_SIBLING){*(uint32_t*)p=((xcb_configure_request_event_t*)e)->sibling;p+=4;}
		if(mz=((xcb_configure_request_event_t*)e)->value_mask&XCB_CONFIG_WINDOW_STACK_MODE)*(uint32_t*)p=((xcb_configure_request_event_t*)e)->stack_mode;
		xcb_configure_window(d,((xcb_configure_request_event_t*)e)->window,((xcb_configure_request_event_t*)e)->value_mask,buf);
		if(mz){
			p=xcb_query_tree_reply(d,xcb_query_tree_unchecked(d,rt),0);
			int32_t*cl=p+32+((xcb_query_tree_reply_t*)p)->children_len*4;
			for(y=p+32;y<cl;y++){
				for(x=cs+1;x<cz;x++)
					if(*x==*y)goto nono;
				*y=0;
				nono:;
			}
			x=cs;
			for(y=p+32;y<cl;y++)
				if(*y)*++x=*y;
			free(p);
			goto pocus;
		}else goto main;}
	case XCB_MAP_REQUEST:{
		void*p=xcb_get_window_attributes_reply(d,xcb_get_window_attributes_unchecked(d,((xcb_map_request_event_t*)e)->window),0);
		if(((xcb_get_window_attributes_reply_t*)p)->override_redirect){
			free(p);
			goto pocus;
		}
		free(p);
		for(;x>cs;x--)
			if(*x==((xcb_map_request_event_t*)e)->window)goto noflush;
		xcb_map_window(d,*cz++=((xcb_map_request_event_t*)e)->window);
		goto hocus;}
	case XCB_MOTION_NOTIFY:
		*buf=mZ&&((xcb_motion_notify_event_t*)e)->root_x<=mx?:((xcb_motion_notify_event_t*)e)->root_x-mx;
		buf[1]=mZ&&((xcb_motion_notify_event_t*)e)->root_y<=my?:((xcb_motion_notify_event_t*)e)->root_y-my;
		xcb_configure_window(d,*x,mZ?XCB_CONFIG_WINDOW_WIDTH|XCB_CONFIG_WINDOW_HEIGHT:XCB_CONFIG_WINDOW_X|XCB_CONFIG_WINDOW_Y,buf);
		goto main;
	case XCB_BUTTON_RELEASE:
		xcb_ungrab_pointer(d,XCB_CURRENT_TIME);
		goto main;
	case XCB_UNMAP_NOTIFY:unmap:goto*(x==cs?&&noflush:*x==((xcb_unmap_notify_event_t*)e)->window&&--cz>cs+1?&&stack:(x--,&&unmap));
	}
stack:mx=*x;
	for(;x!=y;x+=x<y?:-1)*x=x[x<y?:-1];
	*x=mx;
hocus:x=cz-1;
	xcb_configure_window(d,*x,XCB_CONFIG_WINDOW_STACK_MODE,di);
pocus:xcb_set_input_focus(d,XCB_INPUT_FOCUS_POINTER_ROOT,*x,XCB_CURRENT_TIME);
	if(!(mz&128))goto main;
kcode:switch(mz&=127){
	void*p;
	case 1:case 3:
		p=xcb_grab_pointer_reply(d,xcb_grab_pointer_unchecked(d,0,rt,XCB_EVENT_MASK_BUTTON_RELEASE|XCB_EVENT_MASK_POINTER_MOTION,XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC,XCB_NONE,XCB_NONE,XCB_CURRENT_TIME),0);
		if(((xcb_grab_pointer_reply_t*)p)->status!=XCB_GRAB_STATUS_SUCCESS){
			free(p);
			goto noflush;
		}
		free(p);
		p=xcb_get_geometry_reply(d,xcb_get_geometry_unchecked(d,*y),0);
		mx=((xcb_get_geometry_reply_t*)p)->x;
		my=((xcb_get_geometry_reply_t*)p)->y;
		free(p);
		if(mZ=mz==1){
			p=xcb_query_pointer_reply(d,xcb_query_pointer_unchecked(d,rt),0);
			mx=((xcb_query_pointer_reply_t*)p)->root_x-mx;
			my=((xcb_query_pointer_reply_t*)p)->root_y-my;
			free(p);
		}
		goto noflush;
	case 23:case 49:
		if(cz-cs<3)goto main;
		y=tx;
		tx=mz==23?(y!=cs+1?(y?:x)-1:x):!y||y==x?cs+1:y+1;
		if(y&&y<cz-1){
			*buf=y[mz==23?:-1];
			buf[1]=mz==23;
			xcb_configure_window(d,*y,XCB_CONFIG_WINDOW_SIBLING|XCB_CONFIG_WINDOW_STACK_MODE,buf);
		}
		xcb_configure_window(d,*tx,XCB_CONFIG_WINDOW_STACK_MODE,di);
		goto main;
	case 32:return 0;
	case 44:
		if(cz>cs+1)xcb_configure_window(d,*y,XCB_CONFIG_WINDOW_X|XCB_CONFIG_WINDOW_Y|XCB_CONFIG_WINDOW_WIDTH|XCB_CONFIG_WINDOW_HEIGHT,di);
		goto main;
	case 46:
		if(cz==cs+1)goto main;
		if(tx)goto*(mz|=128,&&xt);
		{xcb_intern_atom_cookie_t c1=xcb_intern_atom_unchecked(d,0,12,"WM_PROTOCOLS"),c2=xcb_intern_atom_unchecked(d,0,16,"WM_DELETE_WINDOW");
		p=xcb_intern_atom_reply(d,c1,0);
		mx=((xcb_intern_atom_reply_t*)p)->atom;
		free(p);
		p=xcb_intern_atom_reply(d,c2,0);}
		my=((xcb_intern_atom_reply_t*)p)->atom;
		free(p);
		p=xcb_get_property_reply(d,xcb_get_property_unchecked(d,0,*y,mx,XCB_ATOM_ATOM,0,-1),0);
		xcb_send_event(d,0,*y,XCB_EVENT_MASK_NO_EVENT,(void*)(xcb_client_message_event_t[]){{.response_type=XCB_CLIENT_MESSAGE,.window=*y,.type=mx,.format=32,.data.data32={my,XCB_CURRENT_TIME}}});
		for(mx=(((xcb_get_property_reply_t*)p)->value_len<<2)-4;mx>=0&&*(uint32_t*)(p+sizeof(xcb_get_property_reply_t)+mx)!=my;mx-=4);
		if(mx==-1)killit:xcb_kill_client(d,*y);
		free(p);
		goto main;
	case 47:goto*(cz==cs+1?&&main:(p=0,&&killit));
	case 24:system("alacritty&");
	default:goto main;
	}
}
