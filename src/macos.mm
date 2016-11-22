/*******************************************************************************

Copyright Alan Garny

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*******************************************************************************/

//==============================================================================
// macOS
//==============================================================================

#include "macos.h"

//==============================================================================

#import <Foundation/Foundation.h>

//==============================================================================

#import <AppKit/NSScreen.h>
#import <AppKit/NSWorkspace.h>

//==============================================================================

const char * macosWallpaper()
{
    // Set the given wallpaper on all our screens
    // Note: this doesn't account for the fact that a user may have several
    //       desktops...

    NSURL *url = [[NSWorkspace sharedWorkspace] desktopImageURLForScreen:[NSScreen mainScreen]];
    NSString *urlString = [url absoluteString];

    return [urlString UTF8String];
}

//==============================================================================

void setMacosWallpaper(const char *pWallpaperFileName)
{
    // Set the given wallpaper on all our screens
    // Note: this doesn't account for the fact that a user may have several
    //       desktops...

    @autoreleasepool {
        NSWorkspace *workspace = [NSWorkspace sharedWorkspace];
        NSURL *url = [NSURL fileURLWithPath:[NSString stringWithUTF8String:pWallpaperFileName]];

        for (NSScreen *screen in [NSScreen screens]) {
            NSDictionary *options = [workspace desktopImageOptionsForScreen:screen];

            [workspace setDesktopImageURL:url forScreen:screen options:options error:nil];
        }
    }
}

//==============================================================================
// End of file
//==============================================================================
