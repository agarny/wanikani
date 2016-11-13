#include "macos.h"

#import <Foundation/Foundation.h>

#import <AppKit/NSScreen.h>
#import <AppKit/NSWorkspace.h>

void setMacosWallpaper(const char *pWallpaperFileName)
{
    // Set the given wallpaper

    @autoreleasepool {
        NSWorkspace *workspace = [NSWorkspace sharedWorkspace];
        NSScreen *screen = [NSScreen mainScreen];
        NSURL *url = [NSURL fileURLWithPath:[NSString stringWithUTF8String:pWallpaperFileName]];
        NSDictionary *opts = [workspace desktopImageOptionsForScreen:screen];

        [workspace setDesktopImageURL:url forScreen:screen options:opts error:nil];
    }
}
