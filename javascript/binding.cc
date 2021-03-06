#include "binding.h"

namespace binding {
  static doc_node_t* convert(Napi::Value value) {
    if (value.IsString()) {
      return doc_literal(value.As<Napi::String>().Utf8Value().c_str());
    }

    Napi::Object object = value.As<Napi::Object>();
    std::string type = object.Get("type").As<Napi::String>().Utf8Value();

    if (type == "align") {
      int32_t n = object.Get("n").As<Napi::Number>();

      if (n == 1) {
        return doc_indent(convert(object.Get("contents")));
      }

      if (n == -1) {
        return doc_dedent(convert(object.Get("contents")));
      }
    }

    if (type == "concat") {
      Napi::Array parts = object.Get("parts").As<Napi::Array>();

      size_t size = parts.Length();
      doc_node_t **converted = (doc_node_t **) doc_alloc(sizeof(doc_node_t *) * size);

      for (size_t idx = 0; idx < size; idx++) {
        converted[idx] = convert(parts.Get(idx));
      }

      return doc_concat(size, converted);
    }

    if (type == "group") {
      return doc_group(convert(object.Get("contents")));
    }

    if (type == "if-break") {
      return doc_if_break(
        convert(object.Get("breakContents")),
        convert(object.Get("flatContents"))
      );
    }

    if (type == "line") {
      if (object.Get("soft").ToBoolean()) {
        return doc_soft_line();
      }

      if (object.Get("literal").ToBoolean()) {
        return doc_literal_line();
      }

      if (object.Get("hard").ToBoolean()) {
        return doc_hard_line();
      }

      return doc_line();
    }

    return NULL;
  }

  Napi::String Print(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
  
    Napi::Object nodeArg = info[0].As<Napi::Object>();
    Napi::Object optionsArg = info[1].As<Napi::Object>();

    Napi::Value tabWidth = optionsArg.Get("tabWidth");
    Napi::Value lineLength = optionsArg.Get("lineLength");

    doc_options_t *options =
      doc_options_make(
        tabWidth.IsUndefined() ? 2 : (uint32_t) tabWidth.ToNumber(),
        lineLength.IsUndefined() ? 80 : (uint32_t) lineLength.ToNumber()
      );

    doc_node_t *node = convert(nodeArg);

    char *printed = doc_print(node, options);
    Napi::String output = Napi::String::New(env, printed);

    doc_options_unmake(options);
    doc_node_unmake(node);
    doc_dealloc(printed);

    return output;
  }

  Napi::Object Init(Napi::Env env, Napi::Object exports)  {
    exports.Set("print", Napi::Function::New(env, Print));
    return exports;
  }

  NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
}
