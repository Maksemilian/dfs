// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: test.proto

#ifndef PROTOBUF_INCLUDED_test_2eproto
#define PROTOBUF_INCLUDED_test_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3007000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3007001 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_test_2eproto

// Internal implementation detail -- do not use these members.
struct TableStruct_test_2eproto {
  static const ::google::protobuf::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::google::protobuf::internal::ParseTable schema[1]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors_test_2eproto();
class T;
class TDefaultTypeInternal;
extern TDefaultTypeInternal _T_default_instance_;
namespace google {
namespace protobuf {
template<> ::T* Arena::CreateMaybeMessage<::T>(Arena*);
}  // namespace protobuf
}  // namespace google

// ===================================================================

class T :
    public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:T) */ {
 public:
  T();
  virtual ~T();

  T(const T& from);

  inline T& operator=(const T& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  T(T&& from) noexcept
    : T() {
    *this = ::std::move(from);
  }

  inline T& operator=(T&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor() {
    return default_instance().GetDescriptor();
  }
  static const T& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const T* internal_default_instance() {
    return reinterpret_cast<const T*>(
               &_T_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  void Swap(T* other);
  friend void swap(T& a, T& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline T* New() const final {
    return CreateMaybeMessage<T>(nullptr);
  }

  T* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<T>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const T& from);
  void MergeFrom(const T& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  #if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  static const char* _InternalParse(const char* begin, const char* end, void* object, ::google::protobuf::internal::ParseContext* ctx);
  ::google::protobuf::internal::ParseFunc _ParseFunc() const final { return _InternalParse; }
  #else
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  #endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(T* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return nullptr;
  }
  inline void* MaybeArenaPtr() const {
    return nullptr;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // uint32 v1 = 1;
  void clear_v1();
  static const int kV1FieldNumber = 1;
  ::google::protobuf::uint32 v1() const;
  void set_v1(::google::protobuf::uint32 value);

  // uint32 v2 = 2;
  void clear_v2();
  static const int kV2FieldNumber = 2;
  ::google::protobuf::uint32 v2() const;
  void set_v2(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:T)
 private:
  class HasBitSetters;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::uint32 v1_;
  ::google::protobuf::uint32 v2_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_test_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// T

// uint32 v1 = 1;
inline void T::clear_v1() {
  v1_ = 0u;
}
inline ::google::protobuf::uint32 T::v1() const {
  // @@protoc_insertion_point(field_get:T.v1)
  return v1_;
}
inline void T::set_v1(::google::protobuf::uint32 value) {
  
  v1_ = value;
  // @@protoc_insertion_point(field_set:T.v1)
}

// uint32 v2 = 2;
inline void T::clear_v2() {
  v2_ = 0u;
}
inline ::google::protobuf::uint32 T::v2() const {
  // @@protoc_insertion_point(field_get:T.v2)
  return v2_;
}
inline void T::set_v2(::google::protobuf::uint32 value) {
  
  v2_ = value;
  // @@protoc_insertion_point(field_set:T.v2)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)


// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // PROTOBUF_INCLUDED_test_2eproto