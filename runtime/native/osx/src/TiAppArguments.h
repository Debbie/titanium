/**
 * This file is part of Appcelerator's Titanium project.
 *
 * Copyright 2008 Appcelerator, Inc.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *    http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. 
 */

#import <Cocoa/Cocoa.h>


@interface TiAppArguments : NSObject {
	NSArray *arguments;
	
	NSString *launchURL;
	NSString *tiAppXml;
	BOOL devLaunch;
	NSString *runtimePath;
	NSMutableDictionary *pluginPaths;
}

@property (copy,readwrite) NSString* launchURL;
@property (copy,readwrite) NSString* tiAppXml;
@property (readwrite) BOOL devLaunch;
@property (copy,readwrite) NSString* runtimePath;

-(id)init;
-(NSEnumerator *) plugins;
-(NSString*) pluginPath:(NSString*)pluginName;

@end
