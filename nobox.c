#include <stdlib.h>
#include <sys/wait.h>
#include <xcb/xcb.h>
void sigchld(int x){
	if(signal(SIGCHLD,sigchld)!=SIG_ERR)
		while(0<waitpid(-1,0,WNOHANG));
}
int main(int argc,char**argv){
	static const uint32_t cwa=XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT|XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY|XCB_EVENT_MASK_STRUCTURE_NOTIFY,sma=XCB_STACK_MODE_ABOVE,di[]={0,0,1680,1050};
	sigchld(0);
	xcb_connection_t*d=xcb_connect(0,0);
	void*p;
	int32_t x,y,mx,my,rt=xcb_setup_roots_iterator(xcb_get_setup(d)).data->root,tx,cs[255];
	uint8_t cz=0,mz,mZ;
	xcb_change_window_attributes(d,rt,XCB_CW_EVENT_MASK,&cwa);
	xcb_grab_key(d,1,rt,0,64,XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC);
	for(tx=64;tx>1;tx>>=3){
		xcb_grab_key(d,1,rt,tx,XCB_GRAB_ANY,XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC);
		xcb_grab_button(d,1,rt,XCB_EVENT_MASK_BUTTON_PRESS,XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC,XCB_NONE,XCB_NONE,XCB_GRAB_ANY,tx);
	}
	tx=-tx;
	main:xcb_flush(d);
	noflush:x=y=cz-1;
	again:switch(((xcb_generic_event_t*)(p=xcb_wait_for_event(d)))->response_type&127){
		case XCB_BUTTON_PRESS:
			for(;x>-1;x--)
				if(cs[x]==((xcb_button_press_event_t*)p)->child){
					if(((xcb_key_press_event_t*)p)->detail==2)goto pocus;
					case XCB_KEY_PRESS:
					mz=128|((xcb_key_press_event_t*)p)->detail;
					my=((xcb_key_press_event_t*)p)->state;
					goto stack;
				}
			goto noflush;
		case XCB_KEY_RELEASE:
			if(((xcb_key_press_event_t*)p)->detail!=64||tx==-1)goto again;
			xt:x=tx;
			tx=-1;
			goto stack;
		case XCB_CONFIGURE_REQUEST:;
			uint32_t c[7],*cp=c;
			for(int i=0;i<5;i++)
				if(((xcb_configure_request_event_t*)p)->value_mask&1<<i)*cp++=*(uint16_t*)(p+16+i*2);
			if(((xcb_configure_request_event_t*)p)->value_mask&XCB_CONFIG_WINDOW_SIBLING)*cp++=((xcb_configure_request_event_t*)p)->sibling;
			if(((xcb_configure_request_event_t*)p)->value_mask&XCB_CONFIG_WINDOW_STACK_MODE){
				*cp=((xcb_configure_request_event_t*)p)->stack_mode;
				for(;x>-1;x--)
					if(cs[x]==((xcb_configure_request_event_t*)p)->window)
						switch(((xcb_configure_request_event_t*)p)->stack_mode){
						case XCB_STACK_MODE_BELOW:y=0;
						case XCB_STACK_MODE_ABOVE:goto stack;
						}
			}
			xcb_configure_window(d,((xcb_configure_request_event_t*)p)->window,((xcb_configure_request_event_t*)p)->value_mask,c);
			goto main;
		case XCB_MAP_REQUEST:
			y=((xcb_map_request_event_t*)p)->window;
			p=xcb_get_window_attributes_reply(d,xcb_get_window_attributes_unchecked(d,y),0);
			if(!p)goto noflush;
			x+=((xcb_get_window_attributes_reply_t*)p)->override_redirect;
			free(p);
			if(x>=cz||cz==255)goto noflush;
			for(;x>-1;x--)
				if(cs[x]==y)goto noflush;
			xcb_map_window(d,cs[cz++]=y);
			goto hocus;
		if(0)case XCB_MOTION_NOTIFY:xcb_configure_window(d,cs[y],mZ?XCB_CONFIG_WINDOW_WIDTH|XCB_CONFIG_WINDOW_HEIGHT:XCB_CONFIG_WINDOW_X|XCB_CONFIG_WINDOW_Y,(int32_t[]){mZ&&((xcb_motion_notify_event_t*)p)->root_x<=mx?:((xcb_motion_notify_event_t*)p)->root_x-mx,mZ&&((xcb_motion_notify_event_t*)p)->root_y<=my?:((xcb_motion_notify_event_t*)p)->root_y-my});
		else case XCB_BUTTON_RELEASE:xcb_ungrab_pointer(d,XCB_CURRENT_TIME);
		goto main;
		case XCB_DESTROY_NOTIFY:case XCB_UNMAP_NOTIFY:unmap:goto*(x<0?&&noflush:cs[x]==((xcb_unmap_notify_event_t*)p)->window&&--cz?&&stack:(x--,&&unmap));
		default:goto again;
		}
		mvsz:
			p=xcb_grab_pointer_reply(d,xcb_grab_pointer_unchecked(d,0,rt,XCB_EVENT_MASK_BUTTON_RELEASE|XCB_EVENT_MASK_POINTER_MOTION,XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC,XCB_NONE,XCB_NONE,XCB_CURRENT_TIME),0);
			y+=((xcb_grab_pointer_reply_t*)p)->status!=XCB_GRAB_STATUS_SUCCESS;
			free(p);
			if(y==cz)goto noflush;
			p=xcb_get_geometry_reply(d,xcb_get_geometry_unchecked(d,cs[y]),0);
			mx=((xcb_get_geometry_reply_t*)p)->x;
			my=((xcb_get_geometry_reply_t*)p)->y;
			free(p);
			if(mZ=mz!=1)goto noflush;
			p=xcb_query_pointer_reply(d,xcb_query_pointer_unchecked(d,rt),0);
			mx=((xcb_query_pointer_reply_t*)p)->root_x-mx;
			my=((xcb_query_pointer_reply_t*)p)->root_y-my;
			free(p);
			goto noflush;
		stack:
			mx=cs[x];
			for(;x!=y;x+=x<y?:-1)cs[x]=cs[x<y?x+1:x-1];
			cs[x]=mx;
			hocus:x=cz-1;
			xcb_configure_window(d,cs[x],XCB_CONFIG_WINDOW_STACK_MODE,&sma);
			pocus:xcb_set_input_focus(d,XCB_INPUT_FOCUS_POINTER_ROOT,cs[x],XCB_CURRENT_TIME);
			if(!(mz&128))goto main;
			switch(mz&=127){
			case 1:goto*(my==XCB_MOD_MASK_1?&&mvsz:&&full);
			case 3:goto*(my==XCB_MOD_MASK_1?&&mvsz:&&shut);
			case 23:case 49:
				if(cz<2)goto main;
				y=tx;
				tx=mz==23?(y?(y==-1?x:y)-1:x):y==x?0:y+1;
				if(y!=-1&&y!=cz-1)xcb_configure_window(d,cs[y],XCB_CONFIG_WINDOW_SIBLING|XCB_CONFIG_WINDOW_STACK_MODE,(uint32_t[]){cs[mz==23?y+1:y-1],mz==23?XCB_STACK_MODE_BELOW:XCB_STACK_MODE_ABOVE});
				xcb_configure_window(d,cs[tx],XCB_CONFIG_WINDOW_STACK_MODE,&sma);
				goto main;
			case 24:goto*(p="urxvt +sb -fn 'xft:monospace-10' -e bash&",&&cmd);
			case 25:goto*(p="thunderbird&",&&cmd);
			case 26:goto*(p="scrot -q 1&",&&cmd);
			case 33:goto*(p="halt",&&cmd);
			case 38:goto*(p="firefox&",&&cmd);
			case 39:goto*(p="scite&",&&cmd);
			case 44:full:
				if(cz)xcb_configure_window(d,cs[y],XCB_CONFIG_WINDOW_X|XCB_CONFIG_WINDOW_Y|XCB_CONFIG_WINDOW_WIDTH|XCB_CONFIG_WINDOW_HEIGHT,di);
				goto main;
			case 46:shut:
				if(!cz)goto main;
				if(tx!=-1)goto*(mz|=128,&&xt);
				p=xcb_intern_atom_reply(d,xcb_intern_atom_unchecked(d,0,12,"WM_PROTOCOLS"),0);
				mx=((xcb_intern_atom_reply_t*)p)->atom;
				free(p);
				p=xcb_intern_atom_reply(d,xcb_intern_atom_unchecked(d,0,16,"WM_DELETE_WINDOW"),0);
				my=((xcb_intern_atom_reply_t*)p)->atom;
				free(p);
				p=xcb_get_property_reply(d,xcb_get_property_unchecked(d,0,cs[y],mx,XCB_ATOM_ATOM,0,-1),0);
				xcb_send_event(d,0,cs[y],XCB_EVENT_MASK_NO_EVENT,(void*)(xcb_client_message_event_t[]){{.response_type=XCB_CLIENT_MESSAGE,.window=cs[y],.type=mx,.format=32,.data.data32={my,XCB_CURRENT_TIME}}});
				for(x=(xcb_get_property_value_length(p)>>2)-1;((uint32_t*)xcb_get_property_value(p))[x]!=my&&x>-1;x--);
				if(x==-1)xcb_kill_client(d,cs[y]);
				free(p);
				goto main;
			case 54:p="urxvt +sb -fn 'xft:monospace-14' -geometry 29x2+500+500 -e sh -c 'date;sleep 1'&";
			cmd:system(p);
			default:goto main;
			}
}
