#import <UIKit/UIKit.h>

// Khai báo hàm C++ từ casioemu_ios.cpp
extern void casioemu_ios_start(const char* model_path);

@interface AppDelegate : UIResponder <UIApplicationDelegate>
@property (strong, nonatomic) UIWindow *window;
@end

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application
    didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {

    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    self.window.backgroundColor = [UIColor blackColor];
    [self.window makeKeyAndVisible];

    // Tìm ROM trong Documents (người dùng copy vào qua Files app)
    NSString* documents = NSSearchPathForDirectoriesInDomains(
        NSDocumentDirectory, NSUserDomainMask, YES).firstObject;

    // Tìm file .bin đầu tiên trong Documents
    NSError* error = nil;
    NSArray* files = [[NSFileManager defaultManager]
        contentsOfDirectoryAtPath:documents error:&error];

    NSString* romPath = nil;
    for (NSString* f in files) {
        if ([f.pathExtension.lowercaseString isEqualToString:@"bin"]) {
            romPath = [documents stringByAppendingPathComponent:f];
            break;
        }
    }

    const char* modelPath = romPath ? [romPath UTF8String] : "";

    // Chạy emulator trên background thread để không block UI thread
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0), ^{
        casioemu_ios_start(modelPath);
    });

    return YES;
}

@end
