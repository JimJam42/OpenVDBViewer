#ifndef COREGLDEFINES_H
#define COREGLDEFINES_H

#ifndef CORE_GL_DEFINED
#define CORE_GL_DEFINED
    #if defined (LINUX) || defined (WIN32)
      #ifdef GL44
        #include "gl_core_4_4.h"
      #endif

      #ifdef GL43
        #include "gl_core_4_3.h"
      #endif
      #ifdef GL42
        #include "gl_core_4_2.h"
      #endif

      #ifdef GL32
        #include "gl_core_3_2.h"
      #endif
    #endif
#endif

#endif // COREGLDEFINES_H
