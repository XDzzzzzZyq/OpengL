#pragma once

#ifdef _DEBUG

#define DEBUG(x) std::cout<<(x)<<"\n";
#define DEBUGS(x) std::cout<__func__<<" : "<<__LINE__<<" : "<<(x)<<"\n";
#define GLDEBUG std::cout<<__func__<<" : "<<__LINE__<<" : "<<glDebug_xdz()<<"\n";

#else

#define DEBUG(x)
#define DEBUGS(x)
#define GLDEBUG

#endif


#define SCREEN_W 1740
#define SCREEN_H 810
#define LOOP(x) for(int i = 0; i < x; i++)
#define LOOP_N(x, n) for(int n = 0; n < x; n++)