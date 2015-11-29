/*

 Ngine v5.0
 
 Module      : OSX specific code.
 Description : Handles Mac/iPhone callbacks from OS
               to application.
               
*/

#import "platform/osx/osx_callbacks.h"


@interface EAGLView (PrivateMethods)
- (void)startUpdateCallback;
- (void)restartUpdateCallback;
- (void)stopUpdateCallback;
@end

@implementation EAGLView

@synthesize animating;
@dynamic animationFrameInterval;

// Method required by iPhone
+ (Class)layerClass
{
 return [CAEAGLLayer class];
}

//The EAGL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder*)coder
{    
 NLogContext& enLog = NLogContext::getInstance();
	
 if (self = [super initWithCoder:coder])
    {
    [self setMultipleTouchEnabled:true];
    
    // Get the layer
    CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;

    eaglLayer.opaque = TRUE;
    eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                   [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
        
    m_gpu = NRenderingContext::getPointer();
    string title = "Ngine 4.0 application.";    
    if (m_gpu->screen.create(!EN_FULLSCREEN,320,480,title) )
       enLog << "OpenGL 2.0 ES Rendering Context created!" << endl;
 
    animating = FALSE;
    displayLinkSupported = FALSE;
    animationFrameInterval = 1;
    displayLink = nil;
    animationTimer = nil;

    // A system version of 3.1 or greater is required to use CADisplayLink. The NSTimer
    // class is used as fallback when it isn't available.
    NSString *reqSysVer = @"3.1";
    NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
    if ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending)
       displayLinkSupported = TRUE;
    }

 return self;
}

- (void)AppStart
{
 [self startUpdateCallback];
 ApplicationMainC(0, NULL);
}

- (void)AppUpdate:(id)sender
{
 m_gpu->screen.current();
 mainUpdate();
}

- (void)AppSleep
{
 [self stopUpdateCallback];
 mainSleep();
}

- (void)AppWakeUp
{
 [self startUpdateCallback];
 mainWakeUp();
}

- (void)AppQuit
{
 [self stopUpdateCallback];
 mainQuit();
}

- (void)layoutSubviews
{
 m_gpu->screen.resize((CAEAGLLayer*)self.layer);
 [self AppUpdate:nil];
}

- (NSInteger)animationFrameInterval
{
 return animationFrameInterval;
}

- (void)setAnimationFrameInterval:(NSInteger)frameInterval
{
 // Frame interval defines how many display frames must pass between each time the
 // display link fires. The display link will only fire 30 times a second when the
 // frame internal is two on a display that refreshes 60 times a second. The default
 // frame interval setting of one will fire 60 times a second when the display refreshes
 // at 60 times a second. A frame interval setting of less than one results in undefined
 // behavior.
 if (frameInterval >= 1)
    {
    animationFrameInterval = frameInterval;

    if (animating)
       [self restartUpdateCallback];
    }
}

- (void)startUpdateCallback
{
 if (!animating)
    {
    if (displayLinkSupported)
       {
       // CADisplayLink is API new to iPhone SDK 3.1. Compiling against earlier versions will result in a warning, but can be dismissed
       // if the system version runtime check for CADisplayLink exists in -initWithCoder:. The runtime check ensures this code will
       // not be called in system versions earlier than 3.1.

       displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(AppUpdate:)];
       [displayLink setFrameInterval:animationFrameInterval];
       [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
       }
    else
       animationTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)((1.0 / 60.0) * animationFrameInterval) target:self selector:@selector(AppUpdate:) userInfo:nil repeats:TRUE];

    animating = true;
    }
}

- (void)restartUpdateCallback
{
 if (displayLinkSupported)
    {
    [displayLink invalidate];
    displayLink = nil;
       
    // CADisplayLink is API new to iPhone SDK 3.1. Compiling against earlier versions will result in a warning, but can be dismissed
    // if the system version runtime check for CADisplayLink exists in -initWithCoder:. The runtime check ensures this code will
    // not be called in system versions earlier than 3.1.

    displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(AppUpdate:)];
    [displayLink setFrameInterval:animationFrameInterval];
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    }
 else
    {
    [animationTimer invalidate];
    animationTimer = nil;
    animationTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)((1.0 / 60.0) * animationFrameInterval) target:self selector:@selector(AppUpdate:) userInfo:nil repeats:TRUE];
    }
 animating = true;
}

- (void)stopUpdateCallback
{
 if (animating)
    {
    if (displayLinkSupported)
       {
       [displayLink invalidate];
       displayLink = nil;
       }
    else
       {
       [animationTimer invalidate];
       animationTimer = nil;
       }
    animating = false;
    }
} 

// Touchpad callbacks
// These are four methods touchesBegan, touchesMoved, touchesEnded, touchesCancelled
// and use to notify about touches and gestures
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event 
{
 NInputContext& enInput = NInputContext::getInstance();
  
 // Get touch area resolution
 uint16 width, height;
 enInput.touch.getArea(&width,&height);
 
 // Add all new touches  
 NSArray *alltouches = [touches allObjects];
 for(uint8 i=0; i<[alltouches count]; i++)
    {
    UITouch* touch   = [alltouches objectAtIndex:i];
    CGPoint  normpos = [touch locationInView:self];

    float2 pos;
    pos.x = float(normpos.x) / float(width);
    pos.y = float(normpos.y) / float(height);
    enInput.touch.add(pos);
    }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event 
{
 NInputContext& enInput = NInputContext::getInstance();

 // Get touch area resolution
 uint16 width, height;
 enInput.touch.getArea(&width,&height);
 
 // Update moved touches  
 NSArray *alltouches = [touches allObjects];
 for(uint8 i=0; i<[alltouches count]; i++)
    {
    UITouch* touch   = [alltouches objectAtIndex:i];
    CGPoint  oldPos  = [touch previousLocationInView:self];
    CGPoint  newPos  = [touch locationInView:self];
 
    // From position
    float2 from;
    from.x = float(oldPos.x) / float(width);
    from.y = float(oldPos.y) / float(height);

    // New position
    float2 to;
    to.x = float(newPos.x) / float(width);
    to.y = float(newPos.y) / float(height);
    enInput.touch.move(from, to);
    }

/*
 // Get number of touches
 uint32 counter = [touches count];
 float2 pos;
  
 // TODO: For now just update touches positions, in future store touches moves 
  
 NSArray *alltouches = [touches allObjects];
 for(uint8 i=0; i<[alltouches count]; i++)
    {
    UITouch* touch   = [alltouches objectAtIndex:i];
    CGPoint  normpos = [touch locationInView:self];

    pos.x = (normpos.x / 320.0f);
    pos.y = (normpos.y / 480.0f);
    enInput.touch.setTouch(i,pos);
    }
 enInput.touch.setCount(counter);
*/
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event 
{
 NInputContext& enInput = NInputContext::getInstance();
 
 // Get touch area resolution
 uint16 width, height;
 enInput.touch.getArea(&width,&height);
 
 // Remove touches  
 NSArray *alltouches = [touches allObjects];
 for(uint8 i=0; i<[alltouches count]; i++)
    {
    UITouch* touch   = [alltouches objectAtIndex:i];
    CGPoint  normpos = [touch previousLocationInView:self];

    float2 pos;
    pos.x = float(normpos.x) / float(width);
    pos.y = float(normpos.y) / float(height);
    enInput.touch.release(pos);
    } 
 
 
 
 
 
 
 
/*
 // Get number of touches
 uint32 counter = [touches count];
 float2 pos;
  
 // TODO: For now just update touches positions, in future end touches 
  
 NSArray *alltouches = [touches allObjects];
 for(uint8 i=0; i<[alltouches count]; i++)
    {
    UITouch* touch   = [alltouches objectAtIndex:i];
    CGPoint  normpos = [touch locationInView:self];

    // This function gives last positions of touches that were ended
    // Touches base should be searched for this positions and remove this touches from table
    
    //TODO: For now just clear touches positions, in future store their end positions
    pos.x = 0.0f; // (normpos.x / 320.0f);
    pos.y = 0.0f; // (normpos.y / 480.0f);
    enInput.touch.setTouch(i,pos);
    } 
 enInput.touch.setCount(counter);
*/
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
 NInputContext& enInput = NInputContext::getInstance();

 // Get touch area resolution
 uint16 width, height;
 enInput.touch.getArea(&width,&height);
 
 // Remove touches  
 NSArray *alltouches = [touches allObjects];
 for(uint8 i=0; i<[alltouches count]; i++)
    {
    UITouch* touch   = [alltouches objectAtIndex:i];
    CGPoint  normpos = [touch previousLocationInView:self];

    float2 pos;
    pos.x = float(normpos.x) / float(width);
    pos.y = float(normpos.y) / float(height);
    enInput.touch.discard(pos);
    } 
}

- (void)dealloc
{
 [super dealloc];
}

@end
