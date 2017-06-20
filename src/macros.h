#ifndef NODE_SWORD_SRC_MACROS_H
#define NODE_SWORD_SRC_MACROS_H

#define REQUIRE_ARGUMENT_FUNCTION(i, var, isolate)                             \
    if (args.Length() <= (i) || !args[i]->IsFunction()) {                      \
      isolate->ThrowException(Exception::TypeError(                            \
          String::NewFromUtf8(isolate, "Argument " #i " must be a function"))  \
      );                                                                       \
      return;                                                                  \
    }                                                                          \
    Local<Function> var = Local<Function>::Cast(args[i]);                      \

#define REQUIRE_ARGUMENT_STRING(i, var, isolate)                               \
    if (args.Length() <= (i) || !args[i]->IsString()) {                        \
      isolate->ThrowException(Exception::TypeError(                            \
          String::NewFromUtf8(isolate, "Argument " #i " must be a string"))    \
      );                                                                       \
      return;                                                                  \
    }                                                                          \
    String::Utf8Value var(args[i]->ToString());

#define REQUIRE_ARGUMENT_OBJECT(i, var, isolate)                               \
    if (args.Length() <= (i) || !args[i]->IsObject()) {                        \
        isolate->ThrowException(Exception::TypeError(                          \
            String::NewFromUtf8(isolate, "Argument " #i " must be an object")) \
        );                                                                     \
        return;                                                                \
    }                                                                          \
    Local<Object> var = Local<Object>::Cast(args[i]);

#define TRY_CATCH_CALL(context, callback, argc, argv)                          \
{   TryCatch try_catch;                                                        \
    (callback)->Call((context), (argc), (argv));                               \
    if (try_catch.HasCaught()) {                                               \
        Exception(try_catch);                                                  \
    }                                                                          }
#endif
