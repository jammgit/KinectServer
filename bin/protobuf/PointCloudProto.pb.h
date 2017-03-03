// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: PointCloudProto.proto

#ifndef PROTOBUF_PointCloudProto_2eproto__INCLUDED
#define PROTOBUF_PointCloudProto_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3000000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3000000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace PointCloudProto {

// Internal implementation detail -- do not call these.
void protobuf_AddDesc_PointCloudProto_2eproto();
void protobuf_AssignDesc_PointCloudProto_2eproto();
void protobuf_ShutdownFile_PointCloudProto_2eproto();

class pbRespEnd;
class pbRespStart;

// ===================================================================

class pbRespStart : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:PointCloudProto.pbRespStart) */ {
 public:
  pbRespStart();
  virtual ~pbRespStart();

  pbRespStart(const pbRespStart& from);

  inline pbRespStart& operator=(const pbRespStart& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const pbRespStart& default_instance();

  void Swap(pbRespStart* other);

  // implements Message ----------------------------------------------

  inline pbRespStart* New() const { return New(NULL); }

  pbRespStart* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const pbRespStart& from);
  void MergeFrom(const pbRespStart& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const {
    return InternalSerializeWithCachedSizesToArray(false, output);
  }
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(pbRespStart* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional int32 resultType = 1;
  void clear_resulttype();
  static const int kResultTypeFieldNumber = 1;
  ::google::protobuf::int32 resulttype() const;
  void set_resulttype(::google::protobuf::int32 value);

  // optional string failReason = 2;
  void clear_failreason();
  static const int kFailReasonFieldNumber = 2;
  const ::std::string& failreason() const;
  void set_failreason(const ::std::string& value);
  void set_failreason(const char* value);
  void set_failreason(const char* value, size_t size);
  ::std::string* mutable_failreason();
  ::std::string* release_failreason();
  void set_allocated_failreason(::std::string* failreason);

  // optional int32 pcSvrPoint = 3;
  void clear_pcsvrpoint();
  static const int kPcSvrPointFieldNumber = 3;
  ::google::protobuf::int32 pcsvrpoint() const;
  void set_pcsvrpoint(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:PointCloudProto.pbRespStart)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  ::google::protobuf::internal::ArenaStringPtr failreason_;
  ::google::protobuf::int32 resulttype_;
  ::google::protobuf::int32 pcsvrpoint_;
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_PointCloudProto_2eproto();
  friend void protobuf_AssignDesc_PointCloudProto_2eproto();
  friend void protobuf_ShutdownFile_PointCloudProto_2eproto();

  void InitAsDefaultInstance();
  static pbRespStart* default_instance_;
};
// -------------------------------------------------------------------

class pbRespEnd : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:PointCloudProto.pbRespEnd) */ {
 public:
  pbRespEnd();
  virtual ~pbRespEnd();

  pbRespEnd(const pbRespEnd& from);

  inline pbRespEnd& operator=(const pbRespEnd& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const pbRespEnd& default_instance();

  void Swap(pbRespEnd* other);

  // implements Message ----------------------------------------------

  inline pbRespEnd* New() const { return New(NULL); }

  pbRespEnd* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const pbRespEnd& from);
  void MergeFrom(const pbRespEnd& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const {
    return InternalSerializeWithCachedSizesToArray(false, output);
  }
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(pbRespEnd* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional int32 resultType = 1;
  void clear_resulttype();
  static const int kResultTypeFieldNumber = 1;
  ::google::protobuf::int32 resulttype() const;
  void set_resulttype(::google::protobuf::int32 value);

  // optional string failReason = 2;
  void clear_failreason();
  static const int kFailReasonFieldNumber = 2;
  const ::std::string& failreason() const;
  void set_failreason(const ::std::string& value);
  void set_failreason(const char* value);
  void set_failreason(const char* value, size_t size);
  ::std::string* mutable_failreason();
  ::std::string* release_failreason();
  void set_allocated_failreason(::std::string* failreason);

  // @@protoc_insertion_point(class_scope:PointCloudProto.pbRespEnd)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  ::google::protobuf::internal::ArenaStringPtr failreason_;
  ::google::protobuf::int32 resulttype_;
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_PointCloudProto_2eproto();
  friend void protobuf_AssignDesc_PointCloudProto_2eproto();
  friend void protobuf_ShutdownFile_PointCloudProto_2eproto();

  void InitAsDefaultInstance();
  static pbRespEnd* default_instance_;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// pbRespStart

// optional int32 resultType = 1;
inline void pbRespStart::clear_resulttype() {
  resulttype_ = 0;
}
inline ::google::protobuf::int32 pbRespStart::resulttype() const {
  // @@protoc_insertion_point(field_get:PointCloudProto.pbRespStart.resultType)
  return resulttype_;
}
inline void pbRespStart::set_resulttype(::google::protobuf::int32 value) {
  
  resulttype_ = value;
  // @@protoc_insertion_point(field_set:PointCloudProto.pbRespStart.resultType)
}

// optional string failReason = 2;
inline void pbRespStart::clear_failreason() {
  failreason_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& pbRespStart::failreason() const {
  // @@protoc_insertion_point(field_get:PointCloudProto.pbRespStart.failReason)
  return failreason_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void pbRespStart::set_failreason(const ::std::string& value) {
  
  failreason_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:PointCloudProto.pbRespStart.failReason)
}
inline void pbRespStart::set_failreason(const char* value) {
  
  failreason_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:PointCloudProto.pbRespStart.failReason)
}
inline void pbRespStart::set_failreason(const char* value, size_t size) {
  
  failreason_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:PointCloudProto.pbRespStart.failReason)
}
inline ::std::string* pbRespStart::mutable_failreason() {
  
  // @@protoc_insertion_point(field_mutable:PointCloudProto.pbRespStart.failReason)
  return failreason_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* pbRespStart::release_failreason() {
  // @@protoc_insertion_point(field_release:PointCloudProto.pbRespStart.failReason)
  
  return failreason_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void pbRespStart::set_allocated_failreason(::std::string* failreason) {
  if (failreason != NULL) {
    
  } else {
    
  }
  failreason_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), failreason);
  // @@protoc_insertion_point(field_set_allocated:PointCloudProto.pbRespStart.failReason)
}

// optional int32 pcSvrPoint = 3;
inline void pbRespStart::clear_pcsvrpoint() {
  pcsvrpoint_ = 0;
}
inline ::google::protobuf::int32 pbRespStart::pcsvrpoint() const {
  // @@protoc_insertion_point(field_get:PointCloudProto.pbRespStart.pcSvrPoint)
  return pcsvrpoint_;
}
inline void pbRespStart::set_pcsvrpoint(::google::protobuf::int32 value) {
  
  pcsvrpoint_ = value;
  // @@protoc_insertion_point(field_set:PointCloudProto.pbRespStart.pcSvrPoint)
}

// -------------------------------------------------------------------

// pbRespEnd

// optional int32 resultType = 1;
inline void pbRespEnd::clear_resulttype() {
  resulttype_ = 0;
}
inline ::google::protobuf::int32 pbRespEnd::resulttype() const {
  // @@protoc_insertion_point(field_get:PointCloudProto.pbRespEnd.resultType)
  return resulttype_;
}
inline void pbRespEnd::set_resulttype(::google::protobuf::int32 value) {
  
  resulttype_ = value;
  // @@protoc_insertion_point(field_set:PointCloudProto.pbRespEnd.resultType)
}

// optional string failReason = 2;
inline void pbRespEnd::clear_failreason() {
  failreason_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& pbRespEnd::failreason() const {
  // @@protoc_insertion_point(field_get:PointCloudProto.pbRespEnd.failReason)
  return failreason_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void pbRespEnd::set_failreason(const ::std::string& value) {
  
  failreason_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:PointCloudProto.pbRespEnd.failReason)
}
inline void pbRespEnd::set_failreason(const char* value) {
  
  failreason_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:PointCloudProto.pbRespEnd.failReason)
}
inline void pbRespEnd::set_failreason(const char* value, size_t size) {
  
  failreason_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:PointCloudProto.pbRespEnd.failReason)
}
inline ::std::string* pbRespEnd::mutable_failreason() {
  
  // @@protoc_insertion_point(field_mutable:PointCloudProto.pbRespEnd.failReason)
  return failreason_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* pbRespEnd::release_failreason() {
  // @@protoc_insertion_point(field_release:PointCloudProto.pbRespEnd.failReason)
  
  return failreason_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void pbRespEnd::set_allocated_failreason(::std::string* failreason) {
  if (failreason != NULL) {
    
  } else {
    
  }
  failreason_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), failreason);
  // @@protoc_insertion_point(field_set_allocated:PointCloudProto.pbRespEnd.failReason)
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace PointCloudProto

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_PointCloudProto_2eproto__INCLUDED
