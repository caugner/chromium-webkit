/*	
    WebPluginNullEventSender.h
	Copyright 2002, Apple, Inc. All rights reserved.
*/

#import <Foundation/Foundation.h>
#import <npapi.h>

@class WebPluginView;

@interface WebPluginNullEventSender : NSObject{
    NPP instance;
    NPP_HandleEventProcPtr NPP_HandleEvent;
    bool shouldStop;
    NSWindow *window;
}

-(id)initWithPluginView:(WebPluginView *)pluginView;
-(void)sendNullEvents;
-(void)stop;
@end