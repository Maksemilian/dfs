// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: desctop.proto

#ifndef PROTOBUF_INCLUDED_desctop_2eproto
#define PROTOBUF_INCLUDED_desctop_2eproto

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
#define PROTOBUF_INTERNAL_EXPORT_desctop_2eproto

// Internal implementation detail -- do not use these members.
struct TableStruct_desctop_2eproto {
  static const ::google::protobuf::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::google::protobuf::internal::ParseTable schema[3]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors_desctop_2eproto();
namespace proto {
namespace desctop {
class DesctopInfo;
class DesctopInfoDefaultTypeInternal;
extern DesctopInfoDefaultTypeInternal _DesctopInfo_default_instance_;
class DeviceInfo;
class DeviceInfoDefaultTypeInternal;
extern DeviceInfoDefaultTypeInternal _DeviceInfo_default_instance_;
class DeviceSetInfo;
class DeviceSetInfoDefaultTypeInternal;
extern DeviceSetInfoDefaultTypeInternal _DeviceSetInfo_default_instance_;
}  // namespace desctop
}  // namespace proto
namespace google {
namespace protobuf {
template<> ::proto::desctop::DesctopInfo* Arena::CreateMaybeMessage<::proto::desctop::DesctopInfo>(Arena*);
template<> ::proto::desctop::DeviceInfo* Arena::CreateMaybeMessage<::proto::desctop::DeviceInfo>(Arena*);
template<> ::proto::desctop::DeviceSetInfo* Arena::CreateMaybeMessage<::proto::desctop::DeviceSetInfo>(Arena*);
}  // namespace protobuf
}  // namespace google
namespace proto {
namespace desctop {

// ===================================================================

class DeviceInfo :
    public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:proto.desctop.DeviceInfo) */ {
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

  // @@protoc_insertion_point(class_scope:proto.desctop.DeviceInfo)
 private:
  class HasBitSetters;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr serial_number_;
  ::google::protobuf::internal::ArenaStringPtr interface_type_;
  ::google::protobuf::uint32 ddc_type_count_;
  ::google::protobuf::uint32 channel_count_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_desctop_2eproto;
};
// -------------------------------------------------------------------

class DeviceSetInfo :
    public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:proto.desctop.DeviceSetInfo) */ {
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
    1;

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

  // repeated .proto.desctop.DeviceInfo device_info = 1;
  int device_info_size() const;
  void clear_device_info();
  static const int kDeviceInfoFieldNumber = 1;
  ::proto::desctop::DeviceInfo* mutable_device_info(int index);
  ::google::protobuf::RepeatedPtrField< ::proto::desctop::DeviceInfo >*
      mutable_device_info();
  const ::proto::desctop::DeviceInfo& device_info(int index) const;
  ::proto::desctop::DeviceInfo* add_device_info();
  const ::google::protobuf::RepeatedPtrField< ::proto::desctop::DeviceInfo >&
      device_info() const;

  // @@protoc_insertion_point(class_scope:proto.desctop.DeviceSetInfo)
 private:
  class HasBitSetters;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::RepeatedPtrField< ::proto::desctop::DeviceInfo > device_info_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_desctop_2eproto;
};
// -------------------------------------------------------------------

class DesctopInfo :
    public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:proto.desctop.DesctopInfo) */ {
 public:
  DesctopInfo();
  virtual ~DesctopInfo();

  DesctopInfo(const DesctopInfo& from);

  inline DesctopInfo& operator=(const DesctopInfo& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  DesctopInfo(DesctopInfo&& from) noexcept
    : DesctopInfo() {
    *this = ::std::move(from);
  }

  inline DesctopInfo& operator=(DesctopInfo&& from) noexcept {
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
  static const DesctopInfo& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const DesctopInfo* internal_default_instance() {
    return reinterpret_cast<const DesctopInfo*>(
               &_DesctopInfo_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    2;

  void Swap(DesctopInfo* other);
  friend void swap(DesctopInfo& a, DesctopInfo& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline DesctopInfo* New() const final {
    return CreateMaybeMessage<DesctopInfo>(nullptr);
  }

  DesctopInfo* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<DesctopInfo>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const DesctopInfo& from);
  void MergeFrom(const DesctopInfo& from);
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
  void InternalSwap(DesctopInfo* other);
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

  // repeated .proto.desctop.DeviceSetInfo device_set_info = 1;
  int device_set_info_size() const;
  void clear_device_set_info();
  static const int kDeviceSetInfoFieldNumber = 1;
  ::proto::desctop::DeviceSetInfo* mutable_device_set_info(int index);
  ::google::protobuf::RepeatedPtrField< ::proto::desctop::DeviceSetInfo >*
      mutable_device_set_info();
  const ::proto::desctop::DeviceSetInfo& device_set_info(int index) const;
  ::proto::desctop::DeviceSetInfo* add_device_set_info();
  const ::google::protobuf::RepeatedPtrField< ::proto::desctop::DeviceSetInfo >&
      device_set_info() const;

  // @@protoc_insertion_point(class_scope:proto.desctop.DesctopInfo)
 private:
  class HasBitSetters;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::RepeatedPtrField< ::proto::desctop::DeviceSetInfo > device_set_info_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_desctop_2eproto;
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
  // @@protoc_insertion_point(field_get:proto.desctop.DeviceInfo.serial_number)
  return serial_number_.GetNoArena();
}
inline void DeviceInfo::set_serial_number(const ::std::string& value) {
  
  serial_number_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:proto.desctop.DeviceInfo.serial_number)
}
#if LANG_CXX11
inline void DeviceInfo::set_serial_number(::std::string&& value) {
  
  serial_number_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:proto.desctop.DeviceInfo.serial_number)
}
#endif
inline void DeviceInfo::set_serial_number(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  serial_number_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:proto.desctop.DeviceInfo.serial_number)
}
inline void DeviceInfo::set_serial_number(const char* value, size_t size) {
  
  serial_number_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:proto.desctop.DeviceInfo.serial_number)
}
inline ::std::string* DeviceInfo::mutable_serial_number() {
  
  // @@protoc_insertion_point(field_mutable:proto.desctop.DeviceInfo.serial_number)
  return serial_number_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* DeviceInfo::release_serial_number() {
  // @@protoc_insertion_point(field_release:proto.desctop.DeviceInfo.serial_number)
  
  return serial_number_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void DeviceInfo::set_allocated_serial_number(::std::string* serial_number) {
  if (serial_number != nullptr) {
    
  } else {
    
  }
  serial_number_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), serial_number);
  // @@protoc_insertion_point(field_set_allocated:proto.desctop.DeviceInfo.serial_number)
}

// fixed32 ddc_type_count = 2;
inline void DeviceInfo::clear_ddc_type_count() {
  ddc_type_count_ = 0u;
}
inline ::google::protobuf::uint32 DeviceInfo::ddc_type_count() const {
  // @@protoc_insertion_point(field_get:proto.desctop.DeviceInfo.ddc_type_count)
  return ddc_type_count_;
}
inline void DeviceInfo::set_ddc_type_count(::google::protobuf::uint32 value) {
  
  ddc_type_count_ = value;
  // @@protoc_insertion_point(field_set:proto.desctop.DeviceInfo.ddc_type_count)
}

// fixed32 channel_count = 3;
inline void DeviceInfo::clear_channel_count() {
  channel_count_ = 0u;
}
inline ::google::protobuf::uint32 DeviceInfo::channel_count() const {
  // @@protoc_insertion_point(field_get:proto.desctop.DeviceInfo.channel_count)
  return channel_count_;
}
inline void DeviceInfo::set_channel_count(::google::protobuf::uint32 value) {
  
  channel_count_ = value;
  // @@protoc_insertion_point(field_set:proto.desctop.DeviceInfo.channel_count)
}

// string interface_type = 4;
inline void DeviceInfo::clear_interface_type() {
  interface_type_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& DeviceInfo::interface_type() const {
  // @@protoc_insertion_point(field_get:proto.desctop.DeviceInfo.interface_type)
  return interface_type_.GetNoArena();
}
inline void DeviceInfo::set_interface_type(const ::std::string& value) {
  
  interface_type_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:proto.desctop.DeviceInfo.interface_type)
}
#if LANG_CXX11
inline void DeviceInfo::set_interface_type(::std::string&& value) {
  
  interface_type_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:proto.desctop.DeviceInfo.interface_type)
}
#endif
inline void DeviceInfo::set_interface_type(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  interface_type_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:proto.desctop.DeviceInfo.interface_type)
}
inline void DeviceInfo::set_interface_type(const char* value, size_t size) {
  
  interface_type_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:proto.desctop.DeviceInfo.interface_type)
}
inline ::std::string* DeviceInfo::mutable_interface_type() {
  
  // @@protoc_insertion_point(field_mutable:proto.desctop.DeviceInfo.interface_type)
  return interface_type_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* DeviceInfo::release_interface_type() {
  // @@protoc_insertion_point(field_release:proto.desctop.DeviceInfo.interface_type)
  
  return interface_type_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void DeviceInfo::set_allocated_interface_type(::std::string* interface_type) {
  if (interface_type != nullptr) {
    
  } else {
    
  }
  interface_type_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), interface_type);
  // @@protoc_insertion_point(field_set_allocated:proto.desctop.DeviceInfo.interface_type)
}

// -------------------------------------------------------------------

// DeviceSetInfo

// repeated .proto.desctop.DeviceInfo device_info = 1;
inline int DeviceSetInfo::device_info_size() const {
  return device_info_.size();
}
inline void DeviceSetInfo::clear_device_info() {
  device_info_.Clear();
}
inline ::proto::desctop::DeviceInfo* DeviceSetInfo::mutable_device_info(int index) {
  // @@protoc_insertion_point(field_mutable:proto.desctop.DeviceSetInfo.device_info)
  return device_info_.Mutable(index);
}
inline ::google::protobuf::RepeatedPtrField< ::proto::desctop::DeviceInfo >*
DeviceSetInfo::mutable_device_info() {
  // @@protoc_insertion_point(field_mutable_list:proto.desctop.DeviceSetInfo.device_info)
  return &device_info_;
}
inline const ::proto::desctop::DeviceInfo& DeviceSetInfo::device_info(int index) const {
  // @@protoc_insertion_point(field_get:proto.desctop.DeviceSetInfo.device_info)
  return device_info_.Get(index);
}
inline ::proto::desctop::DeviceInfo* DeviceSetInfo::add_device_info() {
  // @@protoc_insertion_point(field_add:proto.desctop.DeviceSetInfo.device_info)
  return device_info_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::proto::desctop::DeviceInfo >&
DeviceSetInfo::device_info() const {
  // @@protoc_insertion_point(field_list:proto.desctop.DeviceSetInfo.device_info)
  return device_info_;
}

// -------------------------------------------------------------------

// DesctopInfo

// repeated .proto.desctop.DeviceSetInfo device_set_info = 1;
inline int DesctopInfo::device_set_info_size() const {
  return device_set_info_.size();
}
inline void DesctopInfo::clear_device_set_info() {
  device_set_info_.Clear();
}
inline ::proto::desctop::DeviceSetInfo* DesctopInfo::mutable_device_set_info(int index) {
  // @@protoc_insertion_point(field_mutable:proto.desctop.DesctopInfo.device_set_info)
  return device_set_info_.Mutable(index);
}
inline ::google::protobuf::RepeatedPtrField< ::proto::desctop::DeviceSetInfo >*
DesctopInfo::mutable_device_set_info() {
  // @@protoc_insertion_point(field_mutable_list:proto.desctop.DesctopInfo.device_set_info)
  return &device_set_info_;
}
inline const ::proto::desctop::DeviceSetInfo& DesctopInfo::device_set_info(int index) const {
  // @@protoc_insertion_point(field_get:proto.desctop.DesctopInfo.device_set_info)
  return device_set_info_.Get(index);
}
inline ::proto::desctop::DeviceSetInfo* DesctopInfo::add_device_set_info() {
  // @@protoc_insertion_point(field_add:proto.desctop.DesctopInfo.device_set_info)
  return device_set_info_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::proto::desctop::DeviceSetInfo >&
DesctopInfo::device_set_info() const {
  // @@protoc_insertion_point(field_list:proto.desctop.DesctopInfo.device_set_info)
  return device_set_info_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace desctop
}  // namespace proto

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // PROTOBUF_INCLUDED_desctop_2eproto