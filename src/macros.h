#ifndef NODE_SWORD_SRC_MACROS_H
#define NODE_SWORD_SRC_MACROS_H

#define REQUIRE_ARGUMENT_FUNCTION(i, var)                                      \
    if (args.Length() <= (i) || !args[i]->IsFunction()) {                      \
        return ThrowException(Exception::TypeError(                            \
            String::New("Argument " #i " must be a function"))                 \
        );                                                                     \
    }                                                                          \
    Local<Function> var = Local<Function>::Cast(args[i]);


#define REQUIRE_ARGUMENT_STRING(i, var)                                        \
    if (args.Length() <= (i) || !args[i]->IsString()) {                        \
        return ThrowException(Exception::TypeError(                            \
            String::New("Argument " #i " must be a string"))                   \
        );                                                                     \
    }                                                                          \
    String::Utf8Value var(args[i]->ToString());

#define REQUIRE_ARGUMENT_OBJECT(i, var)                                        \
    if (args.Length() <= (i) || !args[i]->IsObject()) {                        \
        return ThrowException(Exception::TypeError(                            \
            String::New("Argument " #i " must be an object"))                   \
        );                                                                     \
    }                                                                          \
    Local<Object> var = Local<Object>::Cast(args[i]);

#define TRY_CATCH_CALL(context, callback, argc, argv)                          \
{   TryCatch try_catch;                                                        \
    (callback)->Call((context), (argc), (argv));                               \
    if (try_catch.HasCaught()) {                                               \
        Exception(try_catch);                                                  \
    }                                                                          }

#endif