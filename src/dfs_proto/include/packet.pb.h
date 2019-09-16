// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: packet.proto

#ifndef PROTOBUF_INCLUDED_packet_2eproto
#define PROTOBUF_INCLUDED_packet_2eproto

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
#define PROTOBUF_INTERNAL_EXPORT_packet_2eproto

// Internal implementation detail -- do not use these members.
struct TableStruct_packet_2eproto {
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
void AddDescriptors_packet_2eproto();
class Packet;
class PacketDefaultTypeInternal;
extern PacketDefaultTypeInternal _Packet_default_instance_;
namespace google {
namespace protobuf {
template<> ::Packet* Arena::CreateMaybeMessage<::Packet>(Arena*);
}  // namespace protobuf
}  // namespace google

// ===================================================================

class Packet :
    public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:Packet) */ {
 public:
  Packet();
  virtual ~Packet();

  Packet(const Packet& from);

  inline Packet& operator=(const Packet& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  Packet(Packet&& from) noexcept
    : Packet() {
    *this = ::std::move(from);
  }

  inline Packet& operator=(Packet&& from) noexcept {
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
  static const Packet& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const Packet* internal_default_instance() {
    return reinterpret_cast<const Packet*>(
               &_Packet_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  void Swap(Packet* other);
  friend void swap(Packet& a, Packet& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline Packet* New() const final {
    return CreateMaybeMessage<Packet>(nullptr);
  }

  Packet* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<Packet>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const Packet& from);
  void MergeFrom(const Packet& from);
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
  void InternalSwap(Packet* other);
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

  // repeated float sample = 11;
  int sample_size() const;
  void clear_sample();
  static const int kSampleFieldNumber = 11;
  float sample(int index) const;
  void set_sample(int index, float value);
  void add_sample(float value);
  const ::google::protobuf::RepeatedField< float >&
      sample() const;
  ::google::protobuf::RepeatedField< float >*
      mutable_sample();

  // uint32 block_number = 1;
  void clear_block_number();
  static const int kBlockNumberFieldNumber = 1;
  ::google::protobuf::uint32 block_number() const;
  void set_block_number(::google::protobuf::uint32 value);

  // uint32 ddc1_frequency = 2;
  void clear_ddc1_frequency();
  static const int kDdc1FrequencyFieldNumber = 2;
  ::google::protobuf::uint32 ddc1_frequency() const;
  void set_ddc1_frequency(::google::protobuf::uint32 value);

  // uint32 attenuator = 3;
  void clear_attenuator();
  static const int kAttenuatorFieldNumber = 3;
  ::google::protobuf::uint32 attenuator() const;
  void set_attenuator(::google::protobuf::uint32 value);

  // uint32 block_size = 4;
  void clear_block_size();
  static const int kBlockSizeFieldNumber = 4;
  ::google::protobuf::uint32 block_size() const;
  void set_block_size(::google::protobuf::uint32 value);

  // uint32 device_count = 5;
  void clear_device_count();
  static const int kDeviceCountFieldNumber = 5;
  ::google::protobuf::uint32 device_count() const;
  void set_device_count(::google::protobuf::uint32 value);

  // uint32 sample_rate = 6;
  void clear_sample_rate();
  static const int kSampleRateFieldNumber = 6;
  ::google::protobuf::uint32 sample_rate() const;
  void set_sample_rate(::google::protobuf::uint32 value);

  // uint32 time_of_week = 7;
  void clear_time_of_week();
  static const int kTimeOfWeekFieldNumber = 7;
  ::google::protobuf::uint32 time_of_week() const;
  void set_time_of_week(::google::protobuf::uint32 value);

  // uint32 week_number = 8;
  void clear_week_number();
  static const int kWeekNumberFieldNumber = 8;
  ::google::protobuf::uint32 week_number() const;
  void set_week_number(::google::protobuf::uint32 value);

  // double ddc_sample_counter = 9;
  void clear_ddc_sample_counter();
  static const int kDdcSampleCounterFieldNumber = 9;
  double ddc_sample_counter() const;
  void set_ddc_sample_counter(double value);

  // uint64 adc_period_counter = 10;
  void clear_adc_period_counter();
  static const int kAdcPeriodCounterFieldNumber = 10;
  ::google::protobuf::uint64 adc_period_counter() const;
  void set_adc_period_counter(::google::protobuf::uint64 value);

  // @@protoc_insertion_point(class_scope:Packet)
 private:
  class HasBitSetters;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::RepeatedField< float > sample_;
  mutable std::atomic<int> _sample_cached_byte_size_;
  ::google::protobuf::uint32 block_number_;
  ::google::protobuf::uint32 ddc1_frequency_;
  ::google::protobuf::uint32 attenuator_;
  ::google::protobuf::uint32 block_size_;
  ::google::protobuf::uint32 device_count_;
  ::google::protobuf::uint32 sample_rate_;
  ::google::protobuf::uint32 time_of_week_;
  ::google::protobuf::uint32 week_number_;
  double ddc_sample_counter_;
  ::google::protobuf::uint64 adc_period_counter_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_packet_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Packet

// uint32 block_number = 1;
inline void Packet::clear_block_number() {
  block_number_ = 0u;
}
inline ::google::protobuf::uint32 Packet::block_number() const {
  // @@protoc_insertion_point(field_get:Packet.block_number)
  return block_number_;
}
inline void Packet::set_block_number(::google::protobuf::uint32 value) {
  
  block_number_ = value;
  // @@protoc_insertion_point(field_set:Packet.block_number)
}

// uint32 ddc1_frequency = 2;
inline void Packet::clear_ddc1_frequency() {
  ddc1_frequency_ = 0u;
}
inline ::google::protobuf::uint32 Packet::ddc1_frequency() const {
  // @@protoc_insertion_point(field_get:Packet.ddc1_frequency)
  return ddc1_frequency_;
}
inline void Packet::set_ddc1_frequency(::google::protobuf::uint32 value) {
  
  ddc1_frequency_ = value;
  // @@protoc_insertion_point(field_set:Packet.ddc1_frequency)
}

// uint32 attenuator = 3;
inline void Packet::clear_attenuator() {
  attenuator_ = 0u;
}
inline ::google::protobuf::uint32 Packet::attenuator() const {
  // @@protoc_insertion_point(field_get:Packet.attenuator)
  return attenuator_;
}
inline void Packet::set_attenuator(::google::protobuf::uint32 value) {
  
  attenuator_ = value;
  // @@protoc_insertion_point(field_set:Packet.attenuator)
}

// uint32 block_size = 4;
inline void Packet::clear_block_size() {
  block_size_ = 0u;
}
inline ::google::protobuf::uint32 Packet::block_size() const {
  // @@protoc_insertion_point(field_get:Packet.block_size)
  return block_size_;
}
inline void Packet::set_block_size(::google::protobuf::uint32 value) {
  
  block_size_ = value;
  // @@protoc_insertion_point(field_set:Packet.block_size)
}

// uint32 device_count = 5;
inline void Packet::clear_device_count() {
  device_count_ = 0u;
}
inline ::google::protobuf::uint32 Packet::device_count() const {
  // @@protoc_insertion_point(field_get:Packet.device_count)
  return device_count_;
}
inline void Packet::set_device_count(::google::protobuf::uint32 value) {
  
  device_count_ = value;
  // @@protoc_insertion_point(field_set:Packet.device_count)
}

// uint32 sample_rate = 6;
inline void Packet::clear_sample_rate() {
  sample_rate_ = 0u;
}
inline ::google::protobuf::uint32 Packet::sample_rate() const {
  // @@protoc_insertion_point(field_get:Packet.sample_rate)
  return sample_rate_;
}
inline void Packet::set_sample_rate(::google::protobuf::uint32 value) {
  
  sample_rate_ = value;
  // @@protoc_insertion_point(field_set:Packet.sample_rate)
}

// uint32 time_of_week = 7;
inline void Packet::clear_time_of_week() {
  time_of_week_ = 0u;
}
inline ::google::protobuf::uint32 Packet::time_of_week() const {
  // @@protoc_insertion_point(field_get:Packet.time_of_week)
  return time_of_week_;
}
inline void Packet::set_time_of_week(::google::protobuf::uint32 value) {
  
  time_of_week_ = value;
  // @@protoc_insertion_point(field_set:Packet.time_of_week)
}

// uint32 week_number = 8;
inline void Packet::clear_week_number() {
  week_number_ = 0u;
}
inline ::google::protobuf::uint32 Packet::week_number() const {
  // @@protoc_insertion_point(field_get:Packet.week_number)
  return week_number_;
}
inline void Packet::set_week_number(::google::protobuf::uint32 value) {
  
  week_number_ = value;
  // @@protoc_insertion_point(field_set:Packet.week_number)
}

// double ddc_sample_counter = 9;
inline void Packet::clear_ddc_sample_counter() {
  ddc_sample_counter_ = 0;
}
inline double Packet::ddc_sample_counter() const {
  // @@protoc_insertion_point(field_get:Packet.ddc_sample_counter)
  return ddc_sample_counter_;
}
inline void Packet::set_ddc_sample_counter(double value) {
  
  ddc_sample_counter_ = value;
  // @@protoc_insertion_point(field_set:Packet.ddc_sample_counter)
}

// uint64 adc_period_counter = 10;
inline void Packet::clear_adc_period_counter() {
  adc_period_counter_ = PROTOBUF_ULONGLONG(0);
}
inline ::google::protobuf::uint64 Packet::adc_period_counter() const {
  // @@protoc_insertion_point(field_get:Packet.adc_period_counter)
  return adc_period_counter_;
}
inline void Packet::set_adc_period_counter(::google::protobuf::uint64 value) {
  
  adc_period_counter_ = value;
  // @@protoc_insertion_point(field_set:Packet.adc_period_counter)
}

// repeated float sample = 11;
inline int Packet::sample_size() const {
  return sample_.size();
}
inline void Packet::clear_sample() {
  sample_.Clear();
}
inline float Packet::sample(int index) const {
  // @@protoc_insertion_point(field_get:Packet.sample)
  return sample_.Get(index);
}
inline void Packet::set_sample(int index, float value) {
  sample_.Set(index, value);
  // @@protoc_insertion_point(field_set:Packet.sample)
}
inline void Packet::add_sample(float value) {
  sample_.Add(value);
  // @@protoc_insertion_point(field_add:Packet.sample)
}
inline const ::google::protobuf::RepeatedField< float >&
Packet::sample() const {
  // @@protoc_insertion_point(field_list:Packet.sample)
  return sample_;
}
inline ::google::protobuf::RepeatedField< float >*
Packet::mutable_sample() {
  // @@protoc_insertion_point(field_mutable_list:Packet.sample)
  return &sample_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)


// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // PROTOBUF_INCLUDED_packet_2eproto
