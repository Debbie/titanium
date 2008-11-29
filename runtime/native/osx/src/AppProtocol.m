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

#include <unistd.h>
#import "AppProtocol.h"
#import "TiAppArguments.h"
#import "TiController.h"

@implementation AppProtocol

+ (NSString*) specialProtocolScheme {
	return @"app";
}


+ (void) registerSpecialProtocol {
	static BOOL inited = NO;
	if ( ! inited ) {
		[NSURLProtocol registerClass:[AppProtocol class]];
		inited = YES;
	}
}

+ (BOOL)canInitWithRequest:(NSURLRequest *)theRequest {
	
    /* get the scheme from the URL */
	NSString *theScheme = [[theRequest URL] scheme];
	
    /* return true if it matches the scheme we're using for our protocol. */
	return ([theScheme caseInsensitiveCompare: [AppProtocol specialProtocolScheme]] == NSOrderedSame );
}


+(NSURLRequest *)canonicalRequestForRequest:(NSURLRequest *)request {
    return request;
}

+ (NSString *)mimeTypeFromExtension:(NSString *)ext
{
	NSString *mime = @"application/octet-stream";
	
	if ([ext isEqualToString:@"png"])
	{
		mime = @"image/png";
	}
	else if ([ext isEqualToString:@"gif"])
	{
		mime = @"image/gif";
	}
	else if ([ext isEqualToString:@"jpg"])
	{
		mime = @"image/jpeg";
	}
	else if ([ext isEqualToString:@"jpeg"])
	{
		mime = @"image/jpeg";
	}
	else if ([ext isEqualToString:@"ico"])
	{
		mime = @"image/x-icon";
	}
	else if ([ext isEqualToString:@"html"])
	{
		mime = @"text/html";
	}
	else if ([ext isEqualToString:@"htm"])
	{
		mime = @"text/html";
	}
	else if ([ext isEqualToString:@"text"])
	{
		mime = @"text/plain";
	}
	else if ([ext isEqualToString:@"js"])
	{
		mime = @"text/javascript";
	}
	return mime;
}

- (void)startLoading
{
    id<NSURLProtocolClient> client = [self client];
    NSURLRequest *request = [self request];
	
	NSURL *url = [request URL];
	NSString *s = [[url absoluteString] substringFromIndex:[[AppProtocol specialProtocolScheme] length]+3];	
	NSString *basePath = [[NSBundle mainBundle] resourcePath];
	NSString *resourcePath = [basePath stringByAppendingPathComponent:s];
	
	TiAppArguments *args = (TiAppArguments *)[[TiController instance] arguments];
	
	if ([args devLaunch]) 
	{
		char *cwd = getcwd(NULL, 0);
		
		NSString *currentPath = [NSString stringWithCString:cwd length:strlen(cwd)];
		resourcePath = [currentPath stringByAppendingPathComponent:s];
	}
	
	NSError *error;
	NSData *data = [NSData dataWithContentsOfFile:resourcePath options:0 error:&error];
	
	if (data == nil)
	{
		// File doesn't exist
		[client URLProtocol:self didFailWithError:error];
		[client URLProtocolDidFinishLoading:self];
		return;
	}
	
	NSString *ext = [resourcePath pathExtension];
	NSString *mime = [AppProtocol mimeTypeFromExtension:ext];
	
	NSURLResponse *response = [[NSURLResponse alloc] initWithURL:url MIMEType:mime expectedContentLength:-1 textEncodingName:@"utf-8"];
	[client URLProtocol:self didReceiveResponse:response cacheStoragePolicy:NSURLCacheStorageAllowed];
	[client URLProtocol:self didLoadData:data];
	[client URLProtocolDidFinishLoading:self];
	[response release];
}

- (void)stopLoading {
	
}

@end