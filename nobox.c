#include <stdlib.h>
#include <sys/wait.h>
#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>
#include <xcb/xcb_icccm.h>
void sigchld(int x){
	if(signal(SIGCHLD,sigchld)!=SIG_ERR)
		while(0<waitpid(-1,0,WNOHANG));
}
int main(int argc,char**argv){
	xcb_connection_t*dpy=xcb_connect(0,0);
	sigchld(0);
	int32_t x,y,mx,my,cs[255],root=xcb_setup_roots_iterator(xcb_get_setup(dpy)).data->root,tx=-1,wmdel=xcb_atom_get(dpy,"WM_DELETE_WINDOW"),wmpro=xcb_atom_get(dpy,"WM_PROTOCOLS");
	xcb_change_window_attributes(dpy,root,XCB_CW_EVENT_MASK,(uint32_t[]){XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT|XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY|XCB_EVENT_MASK_STRUCTURE_NOTIFY});
	uint8_t cz=0,mz;
	xcb_grab_key(dpy,1,root,0,64,XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC);
	for(mz=64;mz>1;mz>>=3){
		xcb_grab_key(dpy,1,root,mz,XCB_GRAB_ANY,XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC);
		xcb_grab_button(dpy,1,root,XCB_EVENT_MASK_BUTTON_PRESS,XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC,XCB_NONE,XCB_NONE,XCB_GRAB_ANY,mz);
	}
	main:
		xcb_flush(dpy);
		noflush:x=y=cz-1;
		void*ret;
		again:switch(((xcb_generic_event_t*)(ret=xcb_wait_for_event(dpy)))->response_type&127){
		case XCB_BUTTON_PRESS:
			for(;x>-1;x--)
				if(cs[x]==((xcb_button_press_event_t*)ret)->child){
					if(((xcb_key_press_event_t*)ret)->detail==2)goto pocus;
					case XCB_KEY_PRESS:
					mz=128|((xcb_key_press_event_t*)ret)->detail;
					my=((xcb_key_press_event_t*)ret)->state;
					goto stack;
				}
			goto noflush;
		case XCB_KEY_RELEASE:
			if(((xcb_key_press_event_t*)ret)->detail!=64||tx==-1)goto again;
			x=tx;
			tx=-1;
			goto stack;
		case XCB_CONFIGURE_REQUEST:{
			xcb_configure_request_event_t*e=ret;
			uint32_t c[6],*p=c;
			if(e->value_mask&XCB_CONFIG_WINDOW_X)*p++=e->x;
			if(e->value_mask&XCB_CONFIG_WINDOW_Y)*p++=e->y;
			if(e->value_mask&XCB_CONFIG_WINDOW_WIDTH)*p++=e->width;
			if(e->value_mask&XCB_CONFIG_WINDOW_HEIGHT)*p++=e->height;
			if(e->value_mask&XCB_CONFIG_WINDOW_SIBLING)*p++=e->sibling;
			if(e->value_mask&XCB_CONFIG_WINDOW_STACK_MODE)*p=e->stack_mode;
			xcb_configure_window(dpy,e->window,e->value_mask&~XCB_CONFIG_WINDOW_BORDER_WIDTH,c);
			for(;x>-1;x--)
				if(cs[x]==e->window&&e->value_mask&XCB_CONFIG_WINDOW_STACK_MODE)
					switch(e->stack_mode){
					case XCB_STACK_MODE_BELOW:y=0;
					case XCB_STACK_MODE_ABOVE:goto stack;
					}
			goto main;
		}
		case XCB_MAP_REQUEST:
			y=((xcb_map_request_event_t*)ret)->window;
			ret=xcb_get_window_attributes_reply(dpy,xcb_get_window_attributes_unchecked(dpy,y),0);
			x+=((xcb_get_window_attributes_reply_t*)ret)->override_redirect;
			free(ret);
			for(;x>-1;x--)
				if(x>=cz||cz==255||cs[x]==y)goto noflush;
			cs[cz++]=y;
			xcb_map_window(dpy,y);
			goto hocus;
		if(0)case XCB_MOTION_NOTIFY:xcb_configure_window(dpy,cs[y],mz==1?XCB_CONFIG_WINDOW_X|XCB_CONFIG_WINDOW_Y:XCB_CONFIG_WINDOW_WIDTH|XCB_CONFIG_WINDOW_HEIGHT,(int32_t[]){mz==3&&((xcb_motion_notify_event_t*)ret)->root_x-mx<1?:((xcb_motion_notify_event_t*)ret)->root_x-mx,mz==3&&((xcb_motion_notify_event_t*)ret)->root_y-my<1?:((xcb_motion_notify_event_t*)ret)->root_y-my});
		else case XCB_BUTTON_RELEASE:xcb_ungrab_pointer(dpy,XCB_CURRENT_TIME);
		goto main;
		case XCB_DESTROY_NOTIFY:case XCB_UNMAP_NOTIFY:unmap:goto*(x<0?&&noflush:cs[x]==((xcb_unmap_notify_event_t*)ret)->window&&--cz?&&stack:(x--,&&unmap));
		default:goto again;
		}
		mvsz:
			ret=xcb_grab_pointer_reply(dpy,xcb_grab_pointer_unchecked(dpy,0,root,XCB_EVENT_MASK_BUTTON_RELEASE|XCB_EVENT_MASK_POINTER_MOTION,XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC,XCB_NONE,XCB_NONE,XCB_CURRENT_TIME),0);
			y+=((xcb_grab_pointer_reply_t*)ret)->status!=XCB_GRAB_STATUS_SUCCESS;
			free(ret);
			ret=xcb_get_geometry_reply(dpy,xcb_get_geometry_unchecked(dpy,cs[y]),0);
			mx=((xcb_get_geometry_reply_t*)ret)->x;
			my=((xcb_get_geometry_reply_t*)ret)->y;
			free(ret);
			if(mz!=1||y==cz)goto main;
			ret=xcb_query_pointer_reply(dpy,xcb_query_pointer_unchecked(dpy,root),0);
			mx=((xcb_query_pointer_reply_t*)ret)->root_x-mx;
			my=((xcb_query_pointer_reply_t*)ret)->root_y-my;
			free(ret);
			goto main;
		stack:
			mx=cs[x];
			for(;x!=y;x+=x<y?:-1)cs[x]=cs[x<y?x+1:x-1];
			cs[x]=mx;
			hocus:x=cz-1;
			pocus:xcb_configure_window(dpy,cs[x],XCB_CONFIG_WINDOW_BORDER_WIDTH|XCB_CONFIG_WINDOW_STACK_MODE,(uint32_t[]){0,XCB_STACK_MODE_ABOVE});
			xcb_set_input_focus(dpy,XCB_INPUT_FOCUS_POINTER_ROOT,cs[x],XCB_CURRENT_TIME);
			if(!(mz&128))goto main;
			switch(mz&=127){
			case 1:goto*(my==XCB_MOD_MASK_1?&&mvsz:&&full);
			case 3:goto*(my==XCB_MOD_MASK_1?&&mvsz:&&shut);
			case 23:case 49:
				if(cz<2)goto main;
				y=tx;
				tx=mz==23?(y?(y==-1?x:y)-1:x):y==x?0:y+1;
				if(y!=-1)xcb_configure_window(dpy,cs[y],XCB_CONFIG_WINDOW_BORDER_WIDTH|XCB_CONFIG_WINDOW_SIBLING|XCB_CONFIG_WINDOW_STACK_MODE,(uint32_t[]){0,cs[tx],XCB_STACK_MODE_ABOVE});
				xcb_configure_window(dpy,cs[tx],XCB_CONFIG_WINDOW_BORDER_WIDTH|XCB_CONFIG_WINDOW_STACK_MODE,(uint32_t[]){1,XCB_STACK_MODE_ABOVE});
				goto main;
			case 24:goto*(ret="uxterm -fa monospace -fs 10&",&&cmd);
			case 25:goto*(ret="uxterm -fa monospace -fs 10 -maximize -e centerim&",&&cmd);
			case 31:goto*(ret="sleep 1;xset dpms force off",&&cmd);
			case 33:goto*(ret="halt",&&cmd);
			case 38:goto*(ret="firefox&",&&cmd);
			case 39:goto*(ret="scite&",&&cmd);
			case 40:goto*(ret="xwd -root>`mktemp --tmpdir=$HOME`&",&&cmd);
			case 44:full:
				if(cz)xcb_configure_window(dpy,cs[y],XCB_CONFIG_WINDOW_X|XCB_CONFIG_WINDOW_Y|XCB_CONFIG_WINDOW_WIDTH|XCB_CONFIG_WINDOW_HEIGHT,(uint32_t[]){0,0,1680,1050});
				goto main;
			case 46:shut:
				if(cz){
					xcb_get_wm_protocols_reply_t pro;
					if(xcb_get_wm_protocols_reply(dpy,xcb_get_wm_protocols_unchecked(dpy,cs[y],wmpro),&pro,0)){
						for(x=pro.atoms_len-1;x>-1;x--)
							if(pro.atoms[x]==wmdel){
								xcb_send_event(dpy,0,cs[y],XCB_EVENT_MASK_NO_EVENT,(char*)(xcb_client_message_event_t[]){{.response_type=XCB_CLIENT_MESSAGE,.window=cs[y],.type=wmpro,.format=32,.data.data32={wmdel,XCB_CURRENT_TIME}}});
								xcb_get_wm_protocols_reply_wipe(&pro);
								goto main;
							}
						xcb_get_wm_protocols_reply_wipe(&pro);
					}
					xcb_kill_client(dpy,cs[y]);
				}
				goto main;
			case 54:goto*(ret="sh -c 'xclock -digital -face monospace -update 1&sleep 2;kill $!'&",&&cmd);
			cmd:system(ret);
			default:goto main;
			}
}
