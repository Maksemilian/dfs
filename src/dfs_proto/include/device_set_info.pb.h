// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: device_set_info.proto

#ifndef PROTOBUF_INCLUDED_device_5fset_5finfo_2eproto
#define PROTOBUF_INCLUDED_device_5fset_5finfo_2eproto

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
#define PROTOBUF_INTERNAL_EXPORT_device_5fset_5finfo_2eproto

// Internal implementation detail -- do not use these members.
struct TableStruct_device_5fset_5finfo_2eproto {
  static const ::google::protobuf::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::google::protobuf::internal::ParseTable schema[4]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors_device_5fset_5finfo_2eproto();
class DeviceInfo;
class DeviceInfoDefaultTypeInternal;
extern DeviceInfoDefaultTypeInternal _DeviceInfo_default_instance_;
class DeviceSetInfo;
class DeviceSetInfoDefaultTypeInternal;
extern DeviceSetInfoDefaultTypeInternal _DeviceSetInfo_default_instance_;
class SignalDirectoryInfo;
class SignalDirectoryInfoDefaultTypeInternal;
extern SignalDirectoryInfoDefaultTypeInternal _SignalDirectoryInfo_default_instance_;
class SignalFileInfo;
class SignalFileInfoDefaultTypeInternal;
extern SignalFileInfoDefaultTypeInternal _SignalFileInfo_default_instance_;
namespace google {
namespace protobuf {
template<> ::DeviceInfo* Arena::CreateMaybeMessage<::DeviceInfo>(Arena*);
template<> ::DeviceSetInfo* Arena::CreateMaybeMessage<::DeviceSetInfo>(Arena*);
template<> ::SignalDirectoryInfo* Arena::CreateMaybeMessage<::SignalDirectoryInfo>(Arena*);
template<> ::SignalFileInfo* Arena::CreateMaybeMessage<::SignalFileInfo>(Arena*);
}  // namespace protobuf
}  // namespace google

// ===================================================================

class DeviceInfo :
    public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:DeviceInfo) */ {
 public:
  DeviceInfo();
  virtual ~DeviceInfo();

  DeviceInfo(const DeviceInfo& from);

  inline DeviceInfo& operator=(const DeviceInfo& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  DeviceInfo(DeviceInfo&& from) noexcept
    : DeviceInfo() {
    *this = ::std::move(from);
  }

  inline DeviceInfo& operator=(DeviceInfo&& from) noexcept {
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
  static const DeviceInfo& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const DeviceInfo* internal_default_instance() {
    return reinterpret_cast<const DeviceInfo*>(
               &_DeviceInfo_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  void Swap(DeviceInfo* other);
  friend void swap(DeviceInfo& a, DeviceInfo& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline DeviceInfo* New() const final {
    return CreateMaybeMessage<DeviceInfo>(nullptr);
  }

  DeviceInfo* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<DeviceInfo>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const DeviceInfo& from);
  void MergeFrom(const DeviceInfo& from);
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
  void InternalSwap(DeviceInfo* other);
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

  // string serial_number = 1;
  void clear_serial_number();
  static const int kSerialNumberFieldNumber = 1;
  const ::std::string& serial_number() const;
  void set_serial_number(const ::std::string& value);
  #if LANG_CXX11
  void set_serial_number(::std::string&& value);
  #endif
  void set_serial_number(const char* value);
  void set_serial_number(const char* value, size_t size);
  ::std::string* mutable_serial_number();
  ::std::string* release_serial_number();
  void set_allocated_serial_number(::std::string* serial_number);

  // string interface_type = 4;
  void clear_interface_type();
  static const int kInterfaceTypeFieldNumber = 4;
  const ::std::string& interface_type() const;
  void set_interface_type(const ::std::string& value);
  #if LANG_CXX11
  void set_interface_type(::std::string&& value);
  #endif
  void set_interface_type(const char* value);
  void set_interface_type(const char* value, size_t size);
  ::std::string* mutable_interface_type();
  ::std::string* release_interface_type();
  void set_allocated_interface_type(::std::string* interface_type);

  // fixed32 ddc_type_count = 2;
  void clear_ddc_type_count();
  static const int kDdcTypeCountFieldNumber = 2;
  ::google::protobuf::uint32 ddc_type_count() const;
  void set_ddc_type_count(::google::protobuf::uint32 value);

  // fixed32 channel_count = 3;
  void clear_channel_count();
  static const int kChannelCountFieldNumber = 3;
  ::google::protobuf::uint32 channel_count() const;
  void set_channel_count(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:DeviceInfo)
 private:
  class HasBitSetters;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr serial_number_;
  ::google::protobuf::internal::ArenaStringPtr interface_type_;
  ::google::protobuf::uint32 ddc_type_count_;
  ::google::protobuf::uint32 channel_count_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_device_5fset_5finfo_2eproto;
};
// -------------------------------------------------------------------

class SignalFileInfo :
    public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:SignalFileInfo) */ {
 public:
  SignalFileInfo();
  virtual ~SignalFileInfo();

  SignalFileInfo(const SignalFileInfo& from);

  inline SignalFileInfo& operator=(const SignalFileInfo& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  SignalFileInfo(SignalFileInfo&& from) noexcept
    : SignalFileInfo() {
    *this = ::std::move(from);
  }

  inline SignalFileInfo& operator=(SignalFileInfo&& from) noexcept {
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
  static const SignalFileInfo& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const SignalFileInfo* internal_default_instance() {
    return reinterpret_cast<const SignalFileInfo*>(
               &_SignalFileInfo_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  void Swap(SignalFileInfo* other);
  friend void swap(SignalFileInfo& a, SignalFileInfo& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline SignalFileInfo* New() const final {
    return CreateMaybeMessage<SignalFileInfo>(nullptr);
  }

  SignalFileInfo* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<SignalFileInfo>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const SignalFileInfo& from);
  void MergeFrom(const SignalFileInfo& from);
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
  void InternalSwap(SignalFileInfo* other);
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

  // string name = 1;
  void clear_name();
  static const int kNameFieldNumber = 1;
  const ::std::string& name() const;
  void set_name(const ::std::string& value);
  #if LANG_CXX11
  void set_name(::std::string&& value);
  #endif
  void set_name(const char* value);
  void set_name(const char* value, size_t size);
  ::std::string* mutable_name();
  ::std::string* release_name();
  void set_allocated_name(::std::string* name);

  // uint32 size = 2;
  void clear_size();
  static const int kSizeFieldNumber = 2;
  ::google::protobuf::uint32 size() const;
  void set_size(::google::protobuf::uint32 value);

  // uint32 ddc_frequency = 3;
  void clear_ddc_frequency();
  static const int kDdcFrequencyFieldNumber = 3;
  ::google::protobuf::uint32 ddc_frequency() const;
  void set_ddc_frequency(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:SignalFileInfo)
 private:
  class HasBitSetters;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr name_;
  ::google::protobuf::uint32 size_;
  ::google::protobuf::uint32 ddc_frequency_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_device_5fset_5finfo_2eproto;
};
// -------------------------------------------------------------------

class DeviceSetInfo :
    public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:DeviceSetInfo) */ {
 public:
  DeviceSetInfo();
  virtual ~DeviceSetInfo();

  DeviceSetInfo(const DeviceSetInfo& from);

  inline DeviceSetInfo& operator=(const DeviceSetInfo& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  DeviceSetInfo(DeviceSetInfo&& from) noexcept
    : DeviceSetInfo() {
    *this = ::std::move(from);
  }

  inline DeviceSetInfo& operator=(DeviceSetInfo&& from) noexcept {
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
  static const DeviceSetInfo& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const DeviceSetInfo* internal_default_instance() {
    return reinterpret_cast<const DeviceSetInfo*>(
               &_DeviceSetInfo_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    2;

  void Swap(DeviceSetInfo* other);
  friend void swap(DeviceSetInfo& a, DeviceSetInfo& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline DeviceSetInfo* New() const final {
    return CreateMaybeMessage<DeviceSetInfo>(nullptr);
  }

  DeviceSetInfo* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<DeviceSetInfo>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const DeviceSetInfo& from);
  void MergeFrom(const DeviceSetInfo& from);
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
  void InternalSwap(DeviceSetInfo* other);
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

  // repeated .DeviceInfo device_info = 1;
  int device_info_size() const;
  void clear_device_info();
  static const int kDeviceInfoFieldNumber = 1;
  ::DeviceInfo* mutable_device_info(int index);
  ::google::protobuf::RepeatedPtrField< ::DeviceInfo >*
      mutable_device_info();
  const ::DeviceInfo& device_info(int index) const;
  ::DeviceInfo* add_device_info();
  const ::google::protobuf::RepeatedPtrField< ::DeviceInfo >&
      device_info() const;

  // uint32 signal_port = 2;
  void clear_signal_port();
  static const int kSignalPortFieldNumber = 2;
  ::google::protobuf::uint32 signal_port() const;
  void set_signal_port(::google::protobuf::uint32 value);

  // uint32 file_port = 3;
  void clear_file_port();
  static const int kFilePortFieldNumber = 3;
  ::google::protobuf::uint32 file_port() const;
  void set_file_port(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:DeviceSetInfo)
 private:
  class HasBitSetters;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::RepeatedPtrField< ::DeviceInfo > device_info_;
  ::google::protobuf::uint32 signal_port_;
  ::google::protobuf::uint32 file_port_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_device_5fset_5finfo_2eproto;
};
// -------------------------------------------------------------------

class SignalDirectoryInfo :
    public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:SignalDirectoryInfo) */ {
 public:
  SignalDirectoryInfo();
  virtual ~SignalDirectoryInfo();

  SignalDirectoryInfo(const SignalDirectoryInfo& from);

  inline SignalDirectoryInfo& operator=(const SignalDirectoryInfo& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  SignalDirectoryInfo(SignalDirectoryInfo&& from) noexcept
    : SignalDirectoryInfo() {
    *this = ::std::move(from);
  }

  inline SignalDirectoryInfo& operator=(SignalDirectoryInfo&& from) noexcept {
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
  static const SignalDirectoryInfo& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const SignalDirectoryInfo* internal_default_instance() {
    return reinterpret_cast<const SignalDirectoryInfo*>(
               &_SignalDirectoryInfo_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    3;

  void Swap(SignalDirectoryInfo* other);
  friend void swap(SignalDirectoryInfo& a, SignalDirectoryInfo& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline SignalDirectoryInfo* New() const final {
    return CreateMaybeMessage<SignalDirectoryInfo>(nullptr);
  }

  SignalDirectoryInfo* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<SignalDirectoryInfo>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const SignalDirectoryInfo& from);
  void MergeFrom(const SignalDirectoryInfo& from);
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
  void InternalSwap(SignalDirectoryInfo* other);
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

  // repeated .SignalFileInfo signal_file_info = 1;
  int signal_file_info_size() const;
  void clear_signal_file_info();
  static const int kSignalFileInfoFieldNumber = 1;
  ::SignalFileInfo* mutable_signal_file_info(int index);
  ::google::protobuf::RepeatedPtrField< ::SignalFileInfo >*
      mutable_signal_file_info();
  const ::SignalFileInfo& signal_file_info(int index) const;
  ::SignalFileInfo* add_signal_file_info();
  const ::google::protobuf::RepeatedPtrField< ::SignalFileInfo >&
      signal_file_info() const;

  // @@protoc_insertion_point(class_scope:SignalDirectoryInfo)
 private:
  class HasBitSetters;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::RepeatedPtrField< ::SignalFileInfo > signal_file_info_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_device_5fset_5finfo_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// DeviceInfo

// string serial_number = 1;
inline void DeviceInfo::clear_serial_number() {
  serial_number_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& DeviceInfo::serial_number() const {
  // @@protoc_insertion_point(field_get:DeviceInfo.serial_number)
  return serial_number_.GetNoArena();
}
inline void DeviceInfo::set_serial_number(const ::std::string& value) {
  
  serial_number_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:DeviceInfo.serial_number)
}
#if LANG_CXX11
inline void DeviceInfo::set_serial_number(::std::string&& value) {
  
  serial_number_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:DeviceInfo.serial_number)
}
#endif
inline void DeviceInfo::set_serial_number(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  serial_number_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:DeviceInfo.serial_number)
}
inline void DeviceInfo::set_serial_number(const char* value, size_t size) {
  
  serial_number_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:DeviceInfo.serial_number)
}
inline ::std::string* DeviceInfo::mutable_serial_number() {
  
  // @@protoc_insertion_point(field_mutable:DeviceInfo.serial_number)
  return serial_number_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* DeviceInfo::release_serial_number() {
  // @@protoc_insertion_point(field_release:DeviceInfo.serial_number)
  
  return serial_number_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void DeviceInfo::set_allocated_serial_number(::std::string* serial_number) {
  if (serial_number != nullptr) {
    
  } else {
    
  }
  serial_number_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), serial_number);
  // @@protoc_insertion_point(field_set_allocated:DeviceInfo.serial_number)
}

// fixed32 ddc_type_count = 2;
inline void DeviceInfo::clear_ddc_type_count() {
  ddc_type_count_ = 0u;
}
inline ::google::protobuf::uint32 DeviceInfo::ddc_type_count() const {
  // @@protoc_insertion_point(field_get:DeviceInfo.ddc_type_count)
  return ddc_type_count_;
}
inline void DeviceInfo::set_ddc_type_count(::google::protobuf::uint32 value) {
  
  ddc_type_count_ = value;
  // @@protoc_insertion_point(field_set:DeviceInfo.ddc_type_count)
}

// fixed32 channel_count = 3;
inline void DeviceInfo::clear_channel_count() {
  channel_count_ = 0u;
}
inline ::google::protobuf::uint32 DeviceInfo::channel_count() const {
  // @@protoc_insertion_point(field_get:DeviceInfo.channel_count)
  return channel_count_;
}
inline void DeviceInfo::set_channel_count(::google::protobuf::uint32 value) {
  
  channel_count_ = value;
  // @@protoc_insertion_point(field_set:DeviceInfo.channel_count)
}

// string interface_type = 4;
inline void DeviceInfo::clear_interface_type() {
  interface_type_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& DeviceInfo::interface_type() const {
  // @@protoc_insertion_point(field_get:DeviceInfo.interface_type)
  return interface_type_.GetNoArena();
}
inline void DeviceInfo::set_interface_type(const ::std::string& value) {
  
  interface_type_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:DeviceInfo.interface_type)
}
#if LANG_CXX11
inline void DeviceInfo::set_interface_type(::std::string&& value) {
  
  interface_type_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:DeviceInfo.interface_type)
}
#endif
inline void DeviceInfo::set_interface_type(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  interface_type_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:DeviceInfo.interface_type)
}
inline void DeviceInfo::set_interface_type(const char* value, size_t size) {
  
  interface_type_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:DeviceInfo.interface_type)
}
inline ::std::string* DeviceInfo::mutable_interface_type() {
  
  // @@protoc_insertion_point(field_mutable:DeviceInfo.interface_type)
  return interface_type_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* DeviceInfo::release_interface_type() {
  // @@protoc_insertion_point(field_release:DeviceInfo.interface_type)
  
  return interface_type_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void DeviceInfo::set_allocated_interface_type(::std::string* interface_type) {
  if (interface_type != nullptr) {
    
  } else {
    
  }
  interface_type_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), interface_type);
  // @@protoc_insertion_point(field_set_allocated:DeviceInfo.interface_type)
}

// -------------------------------------------------------------------

// SignalFileInfo

// string name = 1;
inline void SignalFileInfo::clear_name() {
  name_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& SignalFileInfo::name() const {
  // @@protoc_insertion_point(field_get:SignalFileInfo.name)
  return name_.GetNoArena();
}
inline void SignalFileInfo::set_name(const ::std::string& value) {
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:SignalFileInfo.name)
}
#if LANG_CXX11
inline void SignalFileInfo::set_name(::std::string&& value) {
  
  name_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:SignalFileInfo.name)
}
#endif
inline void SignalFileInfo::set_name(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:SignalFileInfo.name)
}
inline void SignalFileInfo::set_name(const char* value, size_t size) {
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:SignalFileInfo.name)
}
inline ::std::string* SignalFileInfo::mutable_name() {
  
  // @@protoc_insertion_point(field_mutable:SignalFileInfo.name)
  return name_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* SignalFileInfo::release_name() {
  // @@protoc_insertion_point(field_release:SignalFileInfo.name)
  
  return name_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void SignalFileInfo::set_allocated_name(::std::string* name) {
  if (name != nullptr) {
    
  } else {
    
  }
  name_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), name);
  // @@protoc_insertion_point(field_set_allocated:SignalFileInfo.name)
}

// uint32 size = 2;
inline void SignalFileInfo::clear_size() {
  size_ = 0u;
}
inline ::google::protobuf::uint32 SignalFileInfo::size() const {
  // @@protoc_insertion_point(field_get:SignalFileInfo.size)
  return size_;
}
inline void SignalFileInfo::set_size(::google::protobuf::uint32 value) {
  
  size_ = value;
  // @@protoc_insertion_point(field_set:SignalFileInfo.size)
}

// uint32 ddc_frequency = 3;
inline void SignalFileInfo::clear_ddc_frequency() {
  ddc_frequency_ = 0u;
}
inline ::google::protobuf::uint32 SignalFileInfo::ddc_frequency() const {
  // @@protoc_insertion_point(field_get:SignalFileInfo.ddc_frequency)
  return ddc_frequency_;
}
inline void SignalFileInfo::set_ddc_frequency(::google::protobuf::uint32 value) {
  
  ddc_frequency_ = value;
  // @@protoc_insertion_point(field_set:SignalFileInfo.ddc_frequency)
}

// -------------------------------------------------------------------

// DeviceSetInfo

// repeated .DeviceInfo device_info = 1;
inline int DeviceSetInfo::device_info_size() const {
  return device_info_.size();
}
inline void DeviceSetInfo::clear_device_info() {
  device_info_.Clear();
}
inline ::DeviceInfo* DeviceSetInfo::mutable_device_info(int index) {
  // @@protoc_insertion_point(field_mutable:DeviceSetInfo.device_info)
  return device_info_.Mutable(index);
}
inline ::google::protobuf::RepeatedPtrField< ::DeviceInfo >*
DeviceSetInfo::mutable_device_info() {
  // @@protoc_insertion_point(field_mutable_list:DeviceSetInfo.device_info)
  return &device_info_;
}
inline const ::DeviceInfo& DeviceSetInfo::device_info(int index) const {
  // @@protoc_insertion_point(field_get:DeviceSetInfo.device_info)
  return device_info_.Get(index);
}
inline ::DeviceInfo* DeviceSetInfo::add_device_info() {
  // @@protoc_insertion_point(field_add:DeviceSetInfo.device_info)
  return device_info_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::DeviceInfo >&
DeviceSetInfo::device_info() const {
  // @@protoc_insertion_point(field_list:DeviceSetInfo.device_info)
  return device_info_;
}

// uint32 signal_port = 2;
inline void DeviceSetInfo::clear_signal_port() {
  signal_port_ = 0u;
}
inline ::google::protobuf::uint32 DeviceSetInfo::signal_port() const {
  // @@protoc_insertion_point(field_get:DeviceSetInfo.signal_port)
  return signal_port_;
}
inline void DeviceSetInfo::set_signal_port(::google::protobuf::uint32 value) {
  
  signal_port_ = value;
  // @@protoc_insertion_point(field_set:DeviceSetInfo.signal_port)
}

// uint32 file_port = 3;
inline void DeviceSetInfo::clear_file_port() {
  file_port_ = 0u;
}
inline ::google::protobuf::uint32 DeviceSetInfo::file_port() const {
  // @@protoc_insertion_point(field_get:DeviceSetInfo.file_port)
  return file_port_;
}
inline void DeviceSetInfo::set_file_port(::google::protobuf::uint32 value) {
  
  file_port_ = value;
  // @@protoc_insertion_point(field_set:DeviceSetInfo.file_port)
}

// -------------------------------------------------------------------

// SignalDirectoryInfo

// repeated .SignalFileInfo signal_file_info = 1;
inline int SignalDirectoryInfo::signal_file_info_size() const {
  return signal_file_info_.size();
}
inline void SignalDirectoryInfo::clear_signal_file_info() {
  signal_file_info_.Clear();
}
inline ::SignalFileInfo* SignalDirectoryInfo::mutable_signal_file_info(int index) {
  // @@protoc_insertion_point(field_mutable:SignalDirectoryInfo.signal_file_info)
  return signal_file_info_.Mutable(index);
}
inline ::google::protobuf::RepeatedPtrField< ::SignalFileInfo >*
SignalDirectoryInfo::mutable_signal_file_info() {
  // @@protoc_insertion_point(field_mutable_list:SignalDirectoryInfo.signal_file_info)
  return &signal_file_info_;
}
inline const ::SignalFileInfo& SignalDirectoryInfo::signal_file_info(int index) const {
  // @@protoc_insertion_point(field_get:SignalDirectoryInfo.signal_file_info)
  return signal_file_info_.Get(index);
}
inline ::SignalFileInfo* SignalDirectoryInfo::add_signal_file_info() {
  // @@protoc_insertion_point(field_add:SignalDirectoryInfo.signal_file_info)
  return signal_file_info_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::SignalFileInfo >&
SignalDirectoryInfo::signal_file_info() const {
  // @@protoc_insertion_point(field_list:SignalDirectoryInfo.signal_file_info)
  return signal_file_info_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------

// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)


// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // PROTOBUF_INCLUDED_device_5fset_5finfo_2eproto
