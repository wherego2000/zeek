// See the file "COPYING" in the main distribution directory for copyright.

#pragma once

#include <sys/types.h> // for u_char
#include <vector>
#include <list>
#include <array>
#include <unordered_map>

#include "zeek/Obj.h"
#include "zeek/IntrusivePtr.h"
#include "zeek/Type.h"
#include "zeek/Timer.h"
#include "zeek/Notifier.h"
#include "zeek/Reporter.h"
#include "zeek/net_util.h"
#include "zeek/Dict.h"
#include "zeek/ZVal.h"

// We have four different port name spaces: TCP, UDP, ICMP, and UNKNOWN.
// We distinguish between them based on the bits specified in the *_PORT_MASK
// entries specified below.
#define NUM_PORT_SPACES 4
#define PORT_SPACE_MASK 0x30000

#define TCP_PORT_MASK	0x10000
#define UDP_PORT_MASK	0x20000
#define ICMP_PORT_MASK	0x30000

namespace zeek {
class String;
}
using BroString [[deprecated("Remove in v4.1. Use zeek::String instead.")]] = zeek::String;

ZEEK_FORWARD_DECLARE_NAMESPACED(Frame, zeek::detail);
ZEEK_FORWARD_DECLARE_NAMESPACED(Func, zeek);
ZEEK_FORWARD_DECLARE_NAMESPACED(IPAddr, zeek);
ZEEK_FORWARD_DECLARE_NAMESPACED(IPPrefix, zeek);
namespace zeek {
class File;
using FilePtr = zeek::IntrusivePtr<File>;
}
using BroFile [[deprecated("Remove in v4.1. Use zeek::File.")]] = zeek::File;
using BroFilePtr [[deprecated("Remove in v4.1. Use zeek::FilePtr.")]] = zeek::FilePtr;

namespace zeek::detail { class ScriptFunc; }
using BroFunc [[deprecated("Remove in v4.1. Use zeek::detail::ScriptFunc instead.")]] = zeek::detail::ScriptFunc;

ZEEK_FORWARD_DECLARE_NAMESPACED(PrefixTable, zeek::detail);
ZEEK_FORWARD_DECLARE_NAMESPACED(RE_Matcher, zeek);

ZEEK_FORWARD_DECLARE_NAMESPACED(CompositeHash, zeek::detail);
ZEEK_FORWARD_DECLARE_NAMESPACED(HashKey, zeek::detail);

namespace zeek {
namespace run_state {

extern double network_time;
extern double zeek_start_network_time;

}

using FuncPtr = IntrusivePtr<Func>;
using FilePtr = IntrusivePtr<File>;

class Val;
class PortVal;
class AddrVal;
class SubNetVal;
class IntervalVal;
class PatternVal;
class TableVal;
class RecordVal;
class ListVal;
class StringVal;
class EnumVal;
class OpaqueVal;
class VectorVal;
class TableEntryVal;
class TypeVal;

using AddrValPtr = IntrusivePtr<AddrVal>;
using EnumValPtr = IntrusivePtr<EnumVal>;
using ListValPtr = IntrusivePtr<ListVal>;
using PortValPtr = IntrusivePtr<PortVal>;
using RecordValPtr = IntrusivePtr<RecordVal>;
using StringValPtr = IntrusivePtr<StringVal>;
using TableValPtr = IntrusivePtr<TableVal>;
using ValPtr = IntrusivePtr<Val>;
using VectorValPtr = IntrusivePtr<VectorVal>;

class Val : public Obj {
public:
	static inline const ValPtr nil;

	~Val() override;

	Val* Ref()			{ zeek::Ref(this); return this; }
	ValPtr Clone();

	bool IsZero() const;
	bool IsOne() const;

	bro_int_t InternalInt() const;
	bro_uint_t InternalUnsigned() const;
	double InternalDouble() const;

	bro_int_t CoerceToInt() const;
	bro_uint_t CoerceToUnsigned() const;
	double CoerceToDouble() const;

	// Returns a new Val with the "size" of this Val.  What constitutes
	// size depends on the Val's type.
	virtual ValPtr SizeVal() const;

	// Bytes in total value object.
	virtual unsigned int MemoryAllocation() const;

	// Add this value to the given value (if appropriate).
	// Returns true if succcessful.  is_first_init is true only if
	// this is the *first* initialization of the value, not
	// if it's a subsequent += initialization.
	virtual bool AddTo(Val* v, bool is_first_init) const;

	// Remove this value from the given value (if appropriate).
	virtual bool RemoveFrom(Val* v) const;

	[[deprecated("Remove in v4.1.  Use GetType().")]]
	zeek::Type* Type()			{ return type.get(); }
	[[deprecated("Remove in v4.1.  Use GetType().")]]
	const zeek::Type* Type() const	{ return type.get(); }

	const TypePtr& GetType() const
		{ return type; }

	template <class T>
	IntrusivePtr<T> GetType() const
		{ return cast_intrusive<T>(type); }

#define UNDERLYING_ACCESSOR_DECL(ztype, ctype, name) \
	ctype name() const;

UNDERLYING_ACCESSOR_DECL(detail::IntValImplementation, bro_int_t, AsInt)
UNDERLYING_ACCESSOR_DECL(BoolVal, bool, AsBool)
UNDERLYING_ACCESSOR_DECL(EnumVal, int, AsEnum)
UNDERLYING_ACCESSOR_DECL(detail::UnsignedValImplementation, bro_uint_t, AsCount)
UNDERLYING_ACCESSOR_DECL(detail::DoubleValImplementation, double, AsDouble)
UNDERLYING_ACCESSOR_DECL(TimeVal, double, AsTime)
UNDERLYING_ACCESSOR_DECL(IntervalVal, double, AsInterval)
UNDERLYING_ACCESSOR_DECL(AddrVal, const IPAddr&, AsAddr)
UNDERLYING_ACCESSOR_DECL(SubNetVal, const IPPrefix&, AsSubNet)
UNDERLYING_ACCESSOR_DECL(StringVal, const String*, AsString)
UNDERLYING_ACCESSOR_DECL(FuncVal, Func*, AsFunc)
UNDERLYING_ACCESSOR_DECL(FileVal, File*, AsFile)
UNDERLYING_ACCESSOR_DECL(PatternVal, const RE_Matcher*, AsPattern)
UNDERLYING_ACCESSOR_DECL(TableVal, const PDict<TableEntryVal>*, AsTable)
UNDERLYING_ACCESSOR_DECL(TypeVal, zeek::Type*, AsType)

	PatternVal* AsPatternVal();
	const PatternVal* AsPatternVal() const;

	PortVal* AsPortVal();
	const PortVal* AsPortVal() const;

	SubNetVal* AsSubNetVal();
	const SubNetVal* AsSubNetVal() const;

	AddrVal* AsAddrVal();
	const AddrVal* AsAddrVal() const;

	TableVal* AsTableVal();
	const TableVal* AsTableVal() const;

	RecordVal* AsRecordVal();
	const RecordVal* AsRecordVal() const;

	ListVal* AsListVal();
	const ListVal* AsListVal() const;

	StringVal* AsStringVal();
	const StringVal* AsStringVal() const;

	VectorVal* AsVectorVal();
	const VectorVal* AsVectorVal() const;

	EnumVal* AsEnumVal();
	const EnumVal* AsEnumVal() const;

	OpaqueVal* AsOpaqueVal();
	const OpaqueVal* AsOpaqueVal() const;

	void Describe(ODesc* d) const override;
	virtual void DescribeReST(ODesc* d) const;

	// To be overridden by mutable derived class to enable change
	// notification.
	virtual notifier::detail::Modifiable* Modifiable()	{ return nullptr; }

#ifdef DEBUG
	// For debugging, we keep a reference to the global ID to which a
	// value has been bound *last*.
	detail::ID* GetID() const;

	void SetID(detail::ID* id);
#endif

	static bool WouldOverflow(const zeek::Type* from_type, const zeek::Type* to_type, const Val* val);

	TableValPtr GetRecordFields();

	StringValPtr ToJSON(bool only_loggable=false, RE_Matcher* re=nullptr);

	template<typename T>
	T As()
		{
		// Since we're converting from "this", make sure the type requested is a pointer.
		static_assert(std::is_pointer<T>());
		return static_cast<T>(this);
		}

protected:

	// Friends with access to Clone().
	friend class EnumType;
	friend class ListVal;
	friend class RecordVal;
	friend class VectorVal;
	friend class ValManager;
	friend class TableEntryVal;

	virtual void ValDescribe(ODesc* d) const;
	virtual void ValDescribeReST(ODesc* d) const;

	static ValPtr MakeBool(bool b);
	static ValPtr MakeInt(bro_int_t i);
	static ValPtr MakeCount(bro_uint_t u);

	explicit Val(TypePtr t) noexcept
		: type(std::move(t))
		{}

	// For internal use by the Val::Clone() methods.
	struct CloneState {
		// Caches a cloned value for later reuse during the same
		// cloning operation. For recursive types, call this *before*
		// descending down.
		ValPtr NewClone(Val* src, ValPtr dst);

		std::unordered_map<Val*, Val*> clones;
	};

	ValPtr Clone(CloneState* state);
	virtual ValPtr DoClone(CloneState* state);

	TypePtr type;

#ifdef DEBUG
	// For debugging, we keep the name of the ID to which a Val is bound.
	const char* bound_id = nullptr;
#endif

};

// Holds pre-allocated Val objects for those where it's more optimal to
// re-use existing ones rather than allocate anew.
class ValManager {
public:

	static constexpr bro_uint_t PREALLOCATED_COUNTS = 4096;
	static constexpr bro_uint_t PREALLOCATED_INTS = 512;
	static constexpr bro_int_t PREALLOCATED_INT_LOWEST = -255;
	static constexpr bro_int_t PREALLOCATED_INT_HIGHEST =
            PREALLOCATED_INT_LOWEST + PREALLOCATED_INTS - 1;

	ValManager();

	[[deprecated("Remove in v4.1.  Use zeek::val_mgr->True() instead.")]]
	inline Val* GetTrue() const
		{ return b_true->Ref(); }

	inline const ValPtr& True() const
		{ return b_true; }

	[[deprecated("Remove in v4.1.  Use zeek::val_mgr->False() instead.")]]
	inline Val* GetFalse() const
		{ return b_false->Ref(); }

	inline const ValPtr& False() const
		{ return b_false; }

	[[deprecated("Remove in v4.1.  Use zeek::val_mgr->Bool() instead.")]]
	inline Val* GetBool(bool b) const
		{ return b ? b_true->Ref() : b_false->Ref(); }

	inline const ValPtr& Bool(bool b) const
		{ return b ? b_true : b_false; }

	[[deprecated("Remove in v4.1.  Use zeek::val_mgr->Int() instead.")]]
	inline Val* GetInt(int64_t i) const
		{
		return i < PREALLOCATED_INT_LOWEST || i > PREALLOCATED_INT_HIGHEST ?
		    Val::MakeInt(i).release() : ints[i - PREALLOCATED_INT_LOWEST]->Ref();
		}

	inline ValPtr Int(int64_t i) const
		{
		return i < PREALLOCATED_INT_LOWEST || i > PREALLOCATED_INT_HIGHEST ?
		    Val::MakeInt(i) : ints[i - PREALLOCATED_INT_LOWEST];
		}

	[[deprecated("Remove in v4.1.  Use zeek::val_mgr->Count() instead.")]]
	inline Val* GetCount(uint64_t i) const
		{
		return i >= PREALLOCATED_COUNTS ? Val::MakeCount(i).release() : counts[i]->Ref();
		}

	inline ValPtr Count(uint64_t i) const
		{
		return i >= PREALLOCATED_COUNTS ? Val::MakeCount(i) : counts[i];
		}

	[[deprecated("Remove in v4.1.  Use zeek::val_mgr->EmptyString() instead.")]]
	StringVal* GetEmptyString() const;

	inline const StringValPtr& EmptyString() const
		{ return empty_string; }

	// Port number given in host order.
	[[deprecated("Remove in v4.1.  Use zeek::val_mgr->Port() instead.")]]
	PortVal* GetPort(uint32_t port_num, TransportProto port_type) const;

	// Port number given in host order.
	const PortValPtr& Port(uint32_t port_num, TransportProto port_type) const;

	// Host-order port number already masked with port space protocol mask.
	[[deprecated("Remove in v4.1.  Use zeek::val_mgr->Port() instead.")]]
	PortVal* GetPort(uint32_t port_num) const;

	// Host-order port number already masked with port space protocol mask.
	const PortValPtr& Port(uint32_t port_num) const;

private:

	std::array<std::array<PortValPtr, 65536>, NUM_PORT_SPACES> ports;
	std::array<ValPtr, PREALLOCATED_COUNTS> counts;
	std::array<ValPtr, PREALLOCATED_INTS> ints;
	StringValPtr empty_string;
	ValPtr b_true;
	ValPtr b_false;
};

extern ValManager* val_mgr;


namespace detail {

// These are *internal* classes used to allow different publicly visible
// classes to share the same low-level value (per Type::InternalType).
// They may change or go away in the future.

class IntValImplementation : public Val {
public:
	IntValImplementation(TypePtr t, bro_int_t v)
		: Val(std::move(t)), int_val(v)
		{}

	bro_int_t Get() const	{ return int_val; }

protected:
	bro_int_t int_val;
};

class UnsignedValImplementation : public Val {
public:
	UnsignedValImplementation(TypePtr t, bro_uint_t v)
		: Val(std::move(t)), uint_val(v)
		{}

	bro_uint_t Get() const	{ return uint_val; }

protected:
	bro_uint_t uint_val;
};

class DoubleValImplementation : public Val {
public:
	DoubleValImplementation(TypePtr t, double v)
		: Val(std::move(t)), double_val(v)
		{}

	double Get() const	{ return double_val; }

protected:
	double double_val;
};

} // namespace detail

class IntVal final : public detail::IntValImplementation {
public:
	IntVal(bro_int_t v)
		: detail::IntValImplementation(base_type(TYPE_INT), v)
		{}

	// No Get() method since in the current implementation the
	// inherited one serves that role.
};

class BoolVal final : public detail::IntValImplementation {
public:
	BoolVal(bro_int_t v)
		: detail::IntValImplementation(base_type(TYPE_BOOL), v)
		{}
	BoolVal(bool b)
		: BoolVal(bro_int_t(b))
		{}

	bool Get() const	{ return static_cast<bool>(int_val); }
};

class CountVal : public detail::UnsignedValImplementation {
public:
	CountVal(bro_uint_t v)
		: detail::UnsignedValImplementation(base_type(TYPE_COUNT), v)
		{}

	// Same as for IntVal: no Get() method needed.
};

class DoubleVal : public detail::DoubleValImplementation {
public:
	DoubleVal(double v)
		: detail::DoubleValImplementation(base_type(TYPE_DOUBLE), v)
		{}

	// Same as for IntVal: no Get() method needed.
};

#define Microseconds 1e-6
#define Milliseconds 1e-3
#define Seconds 1.0
#define Minutes (60*Seconds)
#define Hours (60*Minutes)
#define Days (24*Hours)

class IntervalVal final : public detail::DoubleValImplementation {
public:
	IntervalVal(double quantity, double units = Seconds)
		: detail::DoubleValImplementation(base_type(TYPE_INTERVAL),
						quantity * units)
		{}

	// Same as for IntVal: no Get() method needed.

protected:
	void ValDescribe(ODesc* d) const override;
};

class TimeVal final : public detail::DoubleValImplementation {
public:
	TimeVal(double t) : detail::DoubleValImplementation(base_type(TYPE_TIME), t)
		{}

	// Same as for IntVal: no Get() method needed.
};

class PortVal final : public detail::UnsignedValImplementation {
public:
	ValPtr SizeVal() const override;

	// Returns the port number in host order (not including the mask).
	uint32_t Port() const;
	std::string Protocol() const;

	// Tests for protocol types.
	bool IsTCP() const;
	bool IsUDP() const;
	bool IsICMP() const;

	TransportProto PortType() const
		{
		if ( IsTCP() )
			return TRANSPORT_TCP;
		else if ( IsUDP() )
			return TRANSPORT_UDP;
		else if ( IsICMP() )
			return TRANSPORT_ICMP;
		else
			return TRANSPORT_UNKNOWN;
		}

	// Returns a masked port number
	static uint32_t Mask(uint32_t port_num, TransportProto port_type);

	// This method is just here to trick the interface in `RecordVal::GetFieldAs` into
	// returning the right type. It shouldn't actually be used for anything.
	zeek::IntrusivePtr<PortVal> Get() { return { NewRef(), AsPortVal() }; }

protected:
	friend class ValManager;
	PortVal(uint32_t p);

	void ValDescribe(ODesc* d) const override;
	ValPtr DoClone(CloneState* state) override;
};

class AddrVal final : public Val {
public:
	explicit AddrVal(const char* text);
	explicit AddrVal(const std::string& text);
	~AddrVal() override;

	ValPtr SizeVal() const override;

	// Constructor for address already in network order.
	explicit AddrVal(uint32_t addr);          // IPv4.
	explicit AddrVal(const uint32_t addr[4]); // IPv6.
	explicit AddrVal(const IPAddr& addr);

	const IPAddr& Get() const	{ return *addr_val; }

	unsigned int MemoryAllocation() const override;

protected:
	ValPtr DoClone(CloneState* state) override;

private:
	IPAddr* addr_val;
};

class SubNetVal final : public Val {
public:
	explicit SubNetVal(const char* text);
	SubNetVal(const char* text, int width);
	SubNetVal(uint32_t addr, int width); // IPv4.
	SubNetVal(const uint32_t addr[4], int width); // IPv6.
	SubNetVal(const IPAddr& addr, int width);
	explicit SubNetVal(const IPPrefix& prefix);
	~SubNetVal() override;

	ValPtr SizeVal() const override;

	const IPAddr& Prefix() const;
	int Width() const;
	IPAddr Mask() const;

	bool Contains(const IPAddr& addr) const;

	const IPPrefix& Get() const	{ return *subnet_val; }

	unsigned int MemoryAllocation() const override;

protected:
	void ValDescribe(ODesc* d) const override;
	ValPtr DoClone(CloneState* state) override;

private:
	IPPrefix* subnet_val;
};

class StringVal final : public Val {
public:
	explicit StringVal(String* s);
	explicit StringVal(const char* s);
	explicit StringVal(const std::string& s);
	StringVal(int length, const char* s);
	~StringVal() override;

	ValPtr SizeVal() const override;

	int Len();
	const u_char* Bytes();
	const char* CheckString();

	// Note that one needs to de-allocate the return value of
	// ExpandedString() to avoid a memory leak.
	// char* ExpandedString(int format = String::EXPANDED_STRING)
	// 	{ return AsString()->ExpandedString(format); }

	std::string ToStdString() const;
	StringVal* ToUpper();

	const String* Get() const	{ return string_val; }

	unsigned int MemoryAllocation() const override;

	StringValPtr Replace(RE_Matcher* re, const String& repl,
	                                      bool do_all);

	[[deprecated("Remove in v4.1.  Use Replace().")]]
	Val* Substitute(RE_Matcher* re, StringVal* repl, bool do_all)
		{ return Replace(re, *repl->AsString(), do_all).release(); }

protected:
	void ValDescribe(ODesc* d) const override;
	ValPtr DoClone(CloneState* state) override;

private:
	String* string_val;
};

class FuncVal final : public Val {
public:
	explicit FuncVal(FuncPtr f);

	FuncPtr AsFuncPtr() const;

	ValPtr SizeVal() const override;

	Func* Get() const	{ return func_val.get(); }

protected:
	void ValDescribe(ODesc* d) const override;
	ValPtr DoClone(CloneState* state) override;

private:
	FuncPtr func_val;
};

class FileVal final : public Val {
public:
	explicit FileVal(FilePtr f);

	ValPtr SizeVal() const override;

	File* Get() const	{ return file_val.get(); }

protected:
	void ValDescribe(ODesc* d) const override;
	ValPtr DoClone(CloneState* state) override;

private:
	FilePtr file_val;
};

class PatternVal final : public Val {
public:
	explicit PatternVal(RE_Matcher* re);
	~PatternVal() override;

	bool AddTo(Val* v, bool is_first_init) const override;

	void SetMatcher(RE_Matcher* re);

	bool MatchExactly(const String* s) const;
	bool MatchAnywhere(const String* s) const;

	const RE_Matcher* Get() const	{ return re_val; }

	unsigned int MemoryAllocation() const override;

protected:
	void ValDescribe(ODesc* d) const override;
	ValPtr DoClone(CloneState* state) override;

private:
	RE_Matcher* re_val;
};

// ListVals are mainly used to index tables that have more than one
// element in their index.
class ListVal final : public Val {
public:
	explicit ListVal(TypeTag t);

	~ListVal() override;

	TypeTag BaseTag() const		{ return tag; }

	ValPtr SizeVal() const override;

	int Length() const		{ return vals.size(); }

	const ValPtr& Idx(size_t i) const	{ return vals[i]; }

	[[deprecated("Remove in v4.1.  Use Idx() instead")]]
	Val* Index(const int n)		{ return vals[n].get(); }
	[[deprecated("Remove in v4.1.  Use Idx() instead")]]
	const Val* Index(const int n) const	{ return vals[n].get(); }

	// Returns an RE_Matcher() that will match any string that
	// includes embedded within it one of the patterns listed
	// (as a string, e.g., "foo|bar") in this ListVal.
	//
	// Assumes that all of the strings in the list are NUL-terminated
	// and do not have any embedded NULs.
	//
	// The return RE_Matcher has not yet been compiled.
	RE_Matcher* BuildRE() const;

	/**
	 * Appends a value to the list.
	 * @param v  the value to append.
	 */
	void Append(ValPtr v);

	[[deprecated("Remove in v4.1.  Use Append(IntrusivePtr) instead.")]]
	void Append(Val* v);

	// Returns a Set representation of the list (which must be homogeneous).
	TableValPtr ToSetVal() const;

	[[deprecated("Remove in v4.1.  Use ToSetVal() instead.")]]
	TableVal* ConvertToSet() const;

	const std::vector<ValPtr>& Vals() const	{ return vals; }

	void Describe(ODesc* d) const override;

	unsigned int MemoryAllocation() const override;

protected:
	ValPtr DoClone(CloneState* state) override;

	std::vector<ValPtr> vals;
	TypeTag tag;
};

class TableEntryVal {
public:
	explicit TableEntryVal(ValPtr v)
		: val(std::move(v))
		{
		expire_access_time =
			int(run_state::network_time - run_state::zeek_start_network_time);
		}

	TableEntryVal* Clone(Val::CloneState* state);

	[[deprecated("Remove in v4.1.  Use GetVal().")]]
	Val* Value()	{ return val.get(); }

	const ValPtr& GetVal() const
		{ return val; }

	// Returns/sets time of last expiration relevant access to this value.
	double ExpireAccessTime() const
		{ return run_state::zeek_start_network_time + expire_access_time; }
	void SetExpireAccess(double time)
		{ expire_access_time = int(time - run_state::zeek_start_network_time); }

protected:
	friend class TableVal;

	ValPtr val;

	// The next entry stores seconds since Zeek's start.  We use ints here
	// to save a few bytes, as we do not need a high resolution for these
	// anyway.
	int expire_access_time;
};

class TableValTimer final : public detail::Timer {
public:
	TableValTimer(TableVal* val, double t);
	~TableValTimer() override;

	void Dispatch(double t, bool is_expire) override;

	TableVal* Table()	{ return table; }

protected:
	TableVal* table;
};

class TableVal final : public Val, public notifier::detail::Modifiable {
public:
	explicit TableVal(TableTypePtr t, detail::AttributesPtr attrs = nullptr);

	[[deprecated("Remove in v4.1.  Construct from IntrusivePtrs instead.")]]
	explicit TableVal(TableType* t, detail::Attributes* attrs = nullptr)
		: TableVal({NewRef{}, t}, {NewRef{}, attrs})
		{}

	~TableVal() override;

	/**
	 * Assigns a value at an associated index in the table (or in the
	 * case of a set, just adds the index).
	 * @param index  The key to assign.
	 * @param new_val  The value to assign at the index.  For a set, this
	 * must be nullptr.
	 * @param broker_forward Controls if the value will be forwarded to attached
	 *        Broker stores.
	 * @param iterators_invalidated  if supplied, gets set to true if the operation
	 *        may have invalidated existing iterators.
	 * @return  True if the assignment type-checked.
	 */
	bool Assign(ValPtr index, ValPtr new_val, bool broker_forward = true,
	            bool* iterators_invalidated = nullptr);

	/**
	 * Assigns a value at an associated index in the table (or in the
	 * case of a set, just adds the index).
	 * @param index  The key to assign.  For tables, this is allowed to be null
	 * (if needed, the index val can be recovered from the hash key).
	 * @param k  A precomputed hash key to use.
	 * @param new_val  The value to assign at the index.  For a set, this
	 * @param iterators_invalidated  if supplied, gets set to true if the operation
	 *        may have invalidated existing iterators.
	 * must be nullptr.
	 * @param broker_forward Controls if the value will be forwarded to attached
	 *        Broker stores.
	 * @return  True if the assignment type-checked.
	 */
	bool Assign(ValPtr index, std::unique_ptr<detail::HashKey> k,
	            ValPtr new_val, bool broker_forward = true,
	            bool* iterators_invalidated = nullptr);

	// Returns true if the assignment typechecked, false if not. The
	// methods take ownership of new_val, but not of the index.  If we're
	// a set, new_val has to be nil.
	[[deprecated("Remove in v4.1.  Use IntrusivePtr overload instead.")]]
	bool Assign(Val* index, Val* new_val);

	// Same as other Assign() method, but takes a precomuted detail::HashKey and
	// deletes it when done.
	[[deprecated("Remove in v4.1.  Use IntrusivePtr overload instead.")]]
	bool Assign(Val* index, detail::HashKey* k, Val* new_val);

	ValPtr SizeVal() const override;

	// Add the entire contents of the table to the given value,
	// which must also be a TableVal.
	// Returns true if the addition typechecked, false if not.
	// If is_first_init is true, then this is the *first* initialization
	// (and so should be strictly adding new elements).
	bool AddTo(Val* v, bool is_first_init) const override;

	// Same but allows suppression of state operations.
	bool AddTo(Val* v, bool is_first_init, bool propagate_ops) const;

	// Remove the entire contents.
	void RemoveAll();

	// Remove the entire contents of the table from the given value.
	// which must also be a TableVal.
	// Returns true if the addition typechecked, false if not.
	bool RemoveFrom(Val* v) const override;

	/**
	 * Returns a new table that is the intersection of this table
	 * and the given table.  Intersection is done only on index, not on
	 * yield value, so this generally makes most sense to use for sets,
	 * not tables.
	 * @param v  The intersecting table.
	 * @return  The intersection of this table and the given one.
	 */
	TableValPtr Intersection(const TableVal& v) const;

	[[deprecated("Remove in v4.1.  Use Intersection() instead.")]]
	TableVal* Intersect(const TableVal* v) const
		{ return Intersection(*v).release(); }

	// Returns true if this set contains the same members as the
	// given set.  Note that comparisons are done using hash keys,
	// so errors can arise for compound sets such as sets-of-sets.
	// See https://bro-tracker.atlassian.net/browse/BIT-1949.
	bool EqualTo(const TableVal& v) const;

	[[deprecated("Remove in v4.1.  Pass TableVal& instead.")]]
	bool EqualTo(const TableVal* v) const
		{ return EqualTo(*v); }

	// Returns true if this set is a subset (not necessarily proper)
	// of the given set.
	bool IsSubsetOf(const TableVal& v) const;

	[[deprecated("Remove in v4.1.  Pass TableVal& instead.")]]
	bool IsSubsetOf(const TableVal* v) const
		{ return IsSubsetOf(*v); }

	// Expands any lists in the index into multiple initializations.
	// Returns true if the initializations typecheck, false if not.
	bool ExpandAndInit(ValPtr index, ValPtr new_val);

	/**
	 * Finds an index in the table and returns its associated value.
	 * @param index  The index to lookup in the table.
	 * @return  The value associated with the index.  If the index doesn't
	 * exist, this is a nullptr.  For sets that don't really contain associated
	 * values, a placeholder value is returned to differentiate it from
	 * non-existent index (nullptr), but otherwise has no meaning in relation
	 * to the set's contents.
	 */
	const ValPtr& Find(const ValPtr& index);

	/**
	 * Finds an index in the table and returns its associated value or else
	 * the &default value.
	 * @param index  The index to lookup in the table.
	 * @return  The value associated with the index.  If the index doesn't
	 * exist, instead returns the &default value.  If there's no &default
	 * attribute, then nullptr is still returned for non-existent index.
	 */
	ValPtr FindOrDefault(const ValPtr& index);

	// Returns the element's value if it exists in the table,
	// nil otherwise.  Note, "index" is not const because we
	// need to Ref/Unref it when calling the default function.
	[[deprecated("Remove in v4.1.  Use Find() or FindOrDefault().")]]
	Val* Lookup(Val* index, bool use_default_val = true);

	/**
	 * Returns true if this is a table[subnet]/set[subnet] and the
	 * given address was found in the table. Otherwise returns false.
	 * @param addr  The address to look for.
	 * @return  Boolean value to indicate if addr is in the table or set. If
	 * self is not a table[subnet]/set[subnet] an internal error will be
	 * generated and false will be returned.
	 */
	bool Contains(const IPAddr& addr) const;

	// For a table[subnet]/set[subnet], return all subnets that cover
	// the given subnet.
	// Causes an internal error if called for any other kind of table.
	VectorValPtr LookupSubnets(const SubNetVal* s);

	// For a set[subnet]/table[subnet], return a new table that only contains
	// entries that cover the given subnet.
	// Causes an internal error if called for any other kind of table.
	TableValPtr LookupSubnetValues(const SubNetVal* s);

	// Sets the timestamp for the given index to network time.
	// Returns false if index does not exist.
	bool UpdateTimestamp(Val* index);

	/**
	 * @return  The index corresponding to the given HashKey.
	 */
	ListValPtr RecreateIndex(const detail::HashKey& k) const;

	[[deprecated("Remove in v4.1.  Use RecreateIndex().")]]
	ListVal* RecoverIndex(const detail::HashKey* k) const
		{ return RecreateIndex(*k).release(); }

	/**
	 * Remove an element from the table and return it.
	 * @param index  The index to remove.
	 * @param broker_forward Controls if the remove operation will be forwarded to attached
	 *        Broker stores.
	 * @param iterators_invalidated  if supplied, gets set to true if the operation
	 *        may have invalidated existing iterators.
	 * @return  The value associated with the index if it exists, else nullptr.
	 * For a sets that don't really contain associated values, a placeholder
	 * value is returned to differentiate it from non-existent index (nullptr),
	 * but otherwise has no meaning in relation to the set's contents.
	 */
	ValPtr Remove(const Val& index, bool broker_forward = true, bool* iterators_invalidated = nullptr);

	/**
	 * Same as Remove(const Val&), but uses a precomputed hash key.
	 * @param k  The hash key to lookup.
	 * @param iterators_invalidated  if supplied, gets set to true if the operation
	 *        may have invalidated existing iterators.
	 * @return  Same as Remove(const Val&).
	 */
	ValPtr Remove(const detail::HashKey& k, bool* iterators_invalidated = nullptr);

	[[deprecated("Remove in v4.1.  Use Remove().")]]
	Val* Delete(const Val* index)
		{ return Remove(*index).release(); }

	[[deprecated("Remove in v4.1.  Use Remove().")]]
	Val* Delete(const detail::HashKey* k)
		{ return Remove(*k).release(); }

	// Returns a ListVal representation of the table (which must be a set).
	ListValPtr ToListVal(TypeTag t = TYPE_ANY) const;

	// Returns a ListVal representation of the table (which must be a set
	// with non-composite index type).
	ListValPtr ToPureListVal() const;

	[[deprecated("Remove in v4.1.  Use ToListVal() instead.")]]
	ListVal* ConvertToList(TypeTag t=TYPE_ANY) const;
	[[deprecated("Remove in v4.1.  Use ToPureListVal() instead.")]]
	ListVal* ConvertToPureList() const;	// must be single index type

	void SetAttrs(detail::AttributesPtr attrs);

	const detail::AttrPtr& GetAttr(detail::AttrTag t) const;

	[[deprecated("Remove in v4.1.  Use GetAttrs().")]]
	detail::Attributes* Attrs()	{ return attrs.get(); }

	const detail::AttributesPtr& GetAttrs() const
		{ return attrs; }

	const PDict<TableEntryVal>* Get() const	{ return table_val; }

	// Returns the size of the table.
	int Size() const;
	int RecursiveSize() const;

	// Returns the Prefix table used inside the table (if present).
	// This allows us to do more direct queries to this specialized
	// type that the general Table API does not allow.
	const detail::PrefixTable* Subnets() const { return subnets; }

	void Describe(ODesc* d) const override;

	void InitTimer(double delay);
	void DoExpire(double t);

	// If the &default attribute is not a function, or the functon has
	// already been initialized, this does nothing. Otherwise, evaluates
	// the function in the frame allowing it to capture its closure.
	void InitDefaultFunc(detail::Frame* f);

	unsigned int MemoryAllocation() const override;

	void ClearTimer(detail::Timer* t)
		{
		if ( timer == t )
			timer = nullptr;
		}

	/**
	 * @param  The index value to hash.
	 * @return  The hash of the index value or nullptr if
	 * type-checking failed.
	 */
	std::unique_ptr<detail::HashKey> MakeHashKey(const Val& index) const;

	[[deprecated("Remove in v4.1.  Use MakeHashKey().")]]
	detail::HashKey* ComputeHash(const Val* index) const;

	notifier::detail::Modifiable* Modifiable() override	{ return this; }

	// Retrieves and saves all table state (key-value pairs) for
	// tables whose index type depends on the given RecordType.
	static void SaveParseTimeTableState(RecordType* rt);

	// Rebuilds all TableVals whose state was previously saved by
	// SaveParseTimeTableState().  This is used to re-recreate the tables
	// in the event that a record type gets redefined while parsing.
	static void RebuildParseTimeTables();

	// Clears all state that was used to track TableVals that depending
	// on RecordTypes.
	static void DoneParsing();

	/**
	 * Sets the name of the Broker store that is backing this table.
	 * @param store store that is backing this table.
	 */
	void SetBrokerStore(const std::string& store) { broker_store = store; }

	/**
	 * Disable change notification processing of &on_change until re-enabled.
	 */
	void DisableChangeNotifications() { in_change_func = true; }

	/**
	 * Re-enables change notifcations after being disabled by DisableChangeNotifications.
	 */
	void EnableChangeNotifications() { in_change_func = false; }

protected:
	void Init(TableTypePtr t);

	using TableRecordDependencies = std::unordered_map<RecordType*, std::vector<TableValPtr>>;

	using ParseTimeTableState = std::vector<std::pair<ValPtr, ValPtr>>;
	using ParseTimeTableStates = std::unordered_map<TableVal*, ParseTimeTableState>;

	ParseTimeTableState DumpTableState();
	void RebuildTable(ParseTimeTableState ptts);

	void CheckExpireAttr(detail::AttrTag at);
	bool ExpandCompoundAndInit(ListVal* lv, int k, ValPtr new_val);
	bool CheckAndAssign(ValPtr index, ValPtr new_val);

	// Calculates default value for index.  Returns nullptr if none.
	ValPtr Default(const ValPtr& index);

	// Returns true if item expiration is enabled.
	bool ExpirationEnabled()	{ return expire_time != nullptr; }

	// Returns the expiration time defined by %{create,read,write}_expire
	// attribute, or -1 for unset/invalid values. In the invalid case, an
	// error will have been reported.
	double GetExpireTime();

	// Calls &expire_func and returns its return interval;
	double CallExpireFunc(ListValPtr idx);

	// Enum for the different kinds of changes an &on_change handler can see
	enum OnChangeType { ELEMENT_NEW, ELEMENT_CHANGED, ELEMENT_REMOVED, ELEMENT_EXPIRED };

	// Calls &change_func.
	void CallChangeFunc(const ValPtr& index, const ValPtr& old_value,
	                    OnChangeType tpe);

	// Sends data on to backing Broker Store
	void SendToStore(const Val* index, const TableEntryVal* new_entry_val, OnChangeType tpe);

	ValPtr DoClone(CloneState* state) override;

	TableTypePtr table_type;
	detail::CompositeHash* table_hash;
	detail::AttributesPtr attrs;
	detail::ExprPtr expire_time;
	detail::ExprPtr expire_func;
	TableValTimer* timer;
	RobustDictIterator* expire_iterator;
	detail::PrefixTable* subnets;
	ValPtr def_val;
	detail::ExprPtr change_func;
	std::string broker_store;
	// prevent recursion of change functions
	bool in_change_func = false;

	static TableRecordDependencies parse_time_table_record_dependencies;
	static ParseTimeTableStates parse_time_table_states;

private:
	PDict<TableEntryVal>* table_val;
};

// This would be way easier with is_convertible_v, but sadly that won't work here
// because Obj has deleted copy constructors (and for good reason). Instead we make
// up our own type trait here that basically combines a bunch of is_same traits
// into a single trait to make life easier in the defintions of GetFieldAs().
template <typename T>
struct is_zeek_val
	{
	static const bool value = std::disjunction_v<
		std::is_same<AddrVal, T>,
		std::is_same<BoolVal, T>,
		std::is_same<CountVal, T>,
		std::is_same<DoubleVal, T>,
		std::is_same<EnumVal, T>,
		std::is_same<FileVal, T>,
		std::is_same<FuncVal, T>,
		std::is_same<IntVal, T>,
		std::is_same<IntervalVal, T>,
		std::is_same<ListVal, T>,
		std::is_same<OpaqueVal, T>,
		std::is_same<PatternVal, T>,
		std::is_same<PortVal, T>,
		std::is_same<RecordVal, T>,
		std::is_same<StringVal, T>,
		std::is_same<SubNetVal, T>,
		std::is_same<TableVal, T>,
		std::is_same<TimeVal, T>,
		std::is_same<TypeVal, T>,
		std::is_same<VectorVal, T>>;
	};
template <typename T>
inline constexpr bool is_zeek_val_v = is_zeek_val<T>::value;

class RecordVal final : public Val, public notifier::detail::Modifiable {
public:
	[[deprecated("Remove in v4.1.  Construct from IntrusivePtr instead.")]]
	explicit RecordVal(RecordType* t, bool init_fields = true);
	explicit RecordVal(RecordTypePtr t, bool init_fields = true);

	~RecordVal() override;

	ValPtr SizeVal() const override;

	/**
	 * Assign a value to a record field.
	 * @param field  The field index to assign.
	 * @param new_val  The value to assign.
	 */
	void Assign(int field, ValPtr new_val);

	/**
	 * Assign a value of type @c T to a record field, as constructed from
	 * the provided arguments.
	 * @param field  The field index to assign.
	 * @param args  A variable number of arguments to pass to constructor of
	 * type @c T.
	 */
	template <class T, class... Ts>
	void Assign(int field, Ts&&... args)
		{ Assign(field, make_intrusive<T>(std::forward<Ts>(args)...)); }

	// Note: the following nullptr method can also go upon removing the above.
	void Assign(int field, std::nullptr_t)
		{ Assign(field, ValPtr{}); }

	/**
	 * Appends a value to the record's fields.  The caller is responsible
	 * for ensuring that fields are appended in the correct order and
	 * with the correct type.
	 * @param v  The value to append.
	 */
	void AppendField(ValPtr v)
		{
		if ( v )
			{
			(*is_in_record)[record_val->size()] = true;
			record_val->emplace_back(ZVal(v, v->GetType()));
			}
		else
			{
			(*is_in_record)[record_val->size()] = false;
			record_val->emplace_back(ZVal());
			}
		}

	/**
	 * Ensures that the record has enough internal storage for the
	 * given number of fields.
	 * @param n  The number of fields.
	 */
	void Reserve(unsigned int n)
		{
		record_val->reserve(n);
		is_in_record->reserve(n);

		for ( auto i = is_in_record->size(); i < n; ++i )
			is_in_record->emplace_back(false);
		}

	/**
	 * Returns the number of fields in the record.
	 * @return  The number of fields in the record.
	 */
	unsigned int NumFields() const
		{ return record_val->size(); }

	/**
	 * Returns the value of a given field index.
	 * @param field  The field index to retrieve.
	 * @return  The value at the given field index.
	 */
	ValPtr GetField(int field) const
		{
		if ( ! (*is_in_record)[field] )
			return nullptr;

		return (*record_val)[field].ToVal(rt->GetFieldType(field));
		}

	/**
	 * Returns the value of a given field index as cast to type @c T.
	 * @param field  The field index to retrieve.
	 * @return  The value at the given field index cast to type @c T.
	 */
	template <class T>
	IntrusivePtr<T> GetField(int field) const
		{ return cast_intrusive<T>(GetField(field)); }

	/**
	 * Returns the value of a given field index if it's previously been
	 * assigned, * or else returns the value created from evaluating the
	 * record field's &default expression.
	 * @param field  The field index to retrieve.
	 * @return  The value at the given field index or the default value if
	 * the field hasn't been assigned yet.
	 */
	ValPtr GetFieldOrDefault(int field) const;

	[[deprecated("Remove in v4.1.  Use GetFieldOrDefault().")]]
	Val* LookupWithDefault(int field) const
		{ return GetFieldOrDefault(field).release(); }

	/**
	 * Returns the value of a given field name.
	 * @param field  The name of a field to retrieve.
	 * @return  The value of the given field.  If no such field name exists,
	 * a fatal error occurs.
	 */
	ValPtr GetField(const char* field) const;

	/**
	 * Returns the value of a given field name as cast to type @c T.
	 * @param field  The name of a field to retrieve.
	 * @return  The value of the given field cast to type @c T.  If no such
	 * field name exists, a fatal error occurs.
	 */
	template <class T>
	IntrusivePtr<T> GetField(const char* field) const
		{ return cast_intrusive<T>(GetField(field)); }

	/**
	 * Returns the value of a given field name if it's previously been
	 * assigned, or else returns the value created from evaluating the record
	 * fields' &default expression.
	 * @param field  The name of a field to retrieve.
	 * @return  The value of the given field.  or the default value
	 * if the field hasn't been assigned yet.  If no such field name exists,
	 * a fatal error occurs.
	 */
	ValPtr GetFieldOrDefault(const char* field) const;

	/**
	 * Returns the value of a given field name or its default value
	 * as cast to type @c T.
	 * @param field  The name of a field to retrieve.
	 * @return  The value of the given field or its default value cast to
	 * type @c T.  If no such field name exists, a fatal error occurs.
	 */
	template <class T>
	IntrusivePtr<T> GetFieldOrDefault(const char* field) const
		{ return cast_intrusive<T>(GetField(field)); }

	// The following return the given field converted to a particular
	// underlying value.  We provide these to enable efficient
	// access to record fields (without requiring an intermediary Val)
	// if we change the underlying representation of records.
	template <typename T,
	          typename std::enable_if_t<is_zeek_val_v<T>, bool> = true>
	auto GetFieldAs(int field) const -> std::invoke_result_t<decltype(&T::Get), T>
		{
		if ( ! (*is_in_record)[field] )
			{
			// TODO: should this be an error via reporter?
			}

		if constexpr ( std::is_same_v<T, BoolVal> ||
		               std::is_same_v<T, IntVal> ||
		               std::is_same_v<T, EnumVal> )
			return record_val->at(field).int_val;
		else if constexpr ( std::is_same_v<T, CountVal> )
			return record_val->at(field).uint_val;
		else if constexpr ( std::is_same_v<T, DoubleVal> ||
		                    std::is_same_v<T, TimeVal> ||
		                    std::is_same_v<T, IntervalVal> )
			return record_val->at(field).double_val;
		else if constexpr ( std::is_same_v<T, PortVal> )
			return val_mgr->Port(record_val->at(field).uint_val);
		else if constexpr ( std::is_same_v<T, StringVal> )
			return record_val->at(field).string_val->Get();
		else if constexpr ( std::is_same_v<T, AddrVal> )
			return record_val->at(field).addr_val->Get();
		else if constexpr ( std::is_same_v<T, SubNetVal> )
			return record_val->at(field).subnet_val->Get();
		else if constexpr ( std::is_same_v<T, File> )
			return *(record_val->at(field).file_val);
		else if constexpr ( std::is_same_v<T, Func> )
			return *(record_val->at(field).func_val);
		else if constexpr ( std::is_same_v<T, PatternVal> )
			return record_val->at(field).re_val->Get();
		else if constexpr ( std::is_same_v<T, TableVal> )
			return record_val->at(field).table_val->Get();
		else
			{
			// TODO: error here, although because of the type trait it really shouldn't
			// ever get here.
			}
		}

	template <typename T,
	          typename std::enable_if_t<!is_zeek_val_v<T>, bool> = true>
	T GetFieldAs(int field) const
		{
		if ( ! (*is_in_record)[field] )
			{
			// TODO: should this be an error via reporter?
			}

		if constexpr ( std::is_integral_v<T> && std::is_signed_v<T> )
			return record_val->at(field).int_val;
		else if constexpr ( std::is_integral_v<T> && std::is_unsigned_v<T> )
			return record_val->at(field).uint_val;
		else if constexpr ( std::is_floating_point_v<T> )
			return record_val->at(field).double_val;

		// TODO: add other types here using type traits, such as is_same_v<T, std::string>, etc

		return T{};
		}

	template <typename T>
	auto GetFieldAs(const char* field) const
		{
		int idx = GetType()->AsRecordType()->FieldOffset(field);

		if ( idx < 0 )
			reporter->InternalError("missing record field: %s", field);

		return GetFieldAs<T>(idx);
		}

	/**
	 * Looks up the value of a field by field name.  If the field doesn't
	 * exist in the record type, it's an internal error: abort.
	 * @param field name of field to lookup.
	 * @param with_default whether to rely on field's &default attribute when
	 * the field has yet to be initialized.
	 * @return the value in field \a field.
	 */
	[[deprecated("Remove in v4.1.  Use GetField() or GetFieldOrDefault().")]]
	Val* Lookup(const char* field, bool with_default = false) const
		{ return with_default ? GetFieldOrDefault(field).release() : GetField(field).get(); }

	void Describe(ODesc* d) const override;

	/**
	 * Returns a "record_field_table" value for introspection purposes.
	 */
	TableValPtr GetRecordFieldsVal() const;

	// This is an experiment to associate a Obj within the
	// event engine to a record value in bro script.
	void SetOrigin(Obj* o)	{ origin = o; }
	Obj* GetOrigin() const	{ return origin; }

	// Returns a new value representing the value coerced to the given
	// type. If coercion is not possible, returns 0. The non-const
	// version may return the current value ref'ed if its type matches
	// directly.
	//
	// *aggr* is optional; if non-zero, we add to it. See
	// Expr::InitVal(). We leave it out in the non-const version to make
	// the choice unambigious.
	//
	// The *allow_orphaning* parameter allows for a record to be demoted
	// down to a record type that contains less fields.
	RecordValPtr CoerceTo(RecordTypePtr other,
	                      RecordValPtr aggr,
	                      bool allow_orphaning = false) const;
	RecordValPtr CoerceTo(RecordTypePtr other,
	                      bool allow_orphaning = false);

	unsigned int MemoryAllocation() const override;
	void DescribeReST(ODesc* d) const override;

	notifier::detail::Modifiable* Modifiable() override	{ return this; }

	// Extend the underlying arrays of record instances created during
	// parsing to match the number of fields in the record type (they may
	// mismatch as a result of parse-time record type redefinitions).
	static void ResizeParseTimeRecords(RecordType* rt);

	static void DoneParsing();

protected:
	ValPtr DoClone(CloneState* state) override;

	Obj* origin;

	using RecordTypeValMap = std::unordered_map<RecordType*, std::vector<RecordValPtr>>;
	static RecordTypeValMap parse_time_records;

private:
	// Keep this handy for quick access during low-level operations.
	RecordTypePtr rt;

	// Low-level values of each of the fields.
	std::vector<ZVal>* record_val;

	// Whether a given field exists - for optional fields, and because
	// Zeek does not enforce that non-optional fields are actually
	// present.
	std::vector<bool>* is_in_record;
};

class EnumVal final : public detail::IntValImplementation {
public:
	ValPtr SizeVal() const override;

protected:
	friend class Val;
	friend class EnumType;

	template<class T, class... Ts>
	friend IntrusivePtr<T> make_intrusive(Ts&&... args);

	EnumVal(EnumTypePtr t, bro_int_t i)
		: detail::IntValImplementation(std::move(t), i)
		{}

	void ValDescribe(ODesc* d) const override;
	ValPtr DoClone(CloneState* state) override;
};

class TypeVal final : public Val {
public:
	TypeVal(TypePtr t) : Val(std::move(t))
		{}

	// Extra arg to differentiate from previous version.
	TypeVal(TypePtr t, bool type_type)
		: Val(make_intrusive<TypeType>(std::move(t)))
		{}

	zeek::Type* Get() const	{ return type.get(); }

protected:
	void ValDescribe(ODesc* d) const override;
	ValPtr DoClone(CloneState* state) override;
};


class VectorVal final : public Val, public notifier::detail::Modifiable {
public:
	[[deprecated("Remove in v4.1.  Construct from IntrusivePtr instead.")]]
	explicit VectorVal(VectorType* t);
	explicit VectorVal(VectorTypePtr t);
	~VectorVal() override;

	ValPtr SizeVal() const override;

	/**
	 * Assigns an element to a given vector index.
	 * @param index  The index to assign.
	 * @param element  The element value to assign.
	 * @return  True if the element was successfully assigned, or false if
	 * the element was the wrong type.
	 */
	bool Assign(unsigned int index, ValPtr element);

	// Note: does NOT Ref() the element! Remember to do so unless
	//       the element was just created and thus has refcount 1.
	[[deprecated("Remove in v4.1.  Assign an IntrusivePtr instead.")]]
	bool Assign(unsigned int index, Val* element)
		{ return Assign(index, {AdoptRef{}, element}); }
	// Note: the following nullptr method can also go upon removing the above.
	void Assign(unsigned int index, std::nullptr_t)
		{ Assign(index, ValPtr{}); }

	[[deprecated("Remove in v4.1.  Assign using integer index and IntrusivePtr element.")]]
	bool Assign(Val* index, Val* element)
		{
		return Assign(index->AsListVal()->Idx(0)->CoerceToUnsigned(),
		              {AdoptRef{}, element});
		}

	/**
	 * Assigns a given value to multiple indices in the vector.
	 * @param index  The starting index to assign to.
	 * @param how_many  The number of indices to assign, counting from *index*.
	 * @return  True if the elements were successfully assigned, or false if
	 * the element was the wrong type.
	 */
	bool AssignRepeat(unsigned int index, unsigned int how_many,
	                  ValPtr element);

	[[deprecated("Remove in v4.1.  Assign an IntrusivePtr instead.")]]
	bool AssignRepeat(unsigned int index, unsigned int how_many, Val* element)
		{ return AssignRepeat(index, how_many, {NewRef{}, element}); }

	// Add this value to the given value (if appropriate).
	// Returns true if succcessful.
	bool AddTo(Val* v, bool is_first_init) const override;

	/**
	 * Returns the given element treated as a Count type, to efficiently
	 * support a common type of vector access if we change the underlying
	 * vector representation.
	 * @param index  The position in the vector of the element to return.
	 * @return  The element's value, as a Count underlying representation.
	 */
	bro_uint_t CountAt(unsigned int index) const
		{ return At(index)->AsCount(); }

	[[deprecated("Remove in v4.1.  Use At().")]]
	Val* Lookup(unsigned int index) const
		{ return At(index).get(); }

	[[deprecated("Remove in v4.1.  Use At().")]]
	Val* Lookup(Val* index)
		{
		bro_uint_t i = index->AsListVal()->Idx(0)->CoerceToUnsigned();
		return At(static_cast<unsigned int>(i)).get();
		}

	unsigned int Size() const { return vector_val->size(); }

	// Is there any way to reclaim previously-allocated memory when you
	// shrink a vector?  The return value is the old size.
	unsigned int Resize(unsigned int new_num_elements);

	// Won't shrink size.
	unsigned int ResizeAtLeast(unsigned int new_num_elements);

	// Reserves storage for at least the number of elements.
	void Reserve(unsigned int num_elements);

	notifier::detail::Modifiable* Modifiable() override	{ return this; }

	/**
	 * Inserts an element at the given position in the vector.  All elements
	 * at that original position and higher are shifted up by one.
	 * @param index  The index to insert the element at.
	 * @param element  The value to insert into the vector.
	 * @return  True if the element was inserted or false if the element was
	 * the wrong type.
	 */
	bool Insert(unsigned int index, ValPtr element);

	[[deprecated("Remove in v4.1.  Insert an IntrusivePtr instead.")]]
	bool Insert(unsigned int index, Val* element)
		{ return Insert(index, {AdoptRef{}, element}); }

	/**
	 * Inserts an element at the end of the vector.
	 * @param element  The value to insert into the vector.
	 * @return  True if the element was inserted or false if the element was
	 * the wrong type.
	 */
	bool Append(ValPtr element)
		{ return Insert(Size(), element); }

	// Removes an element at a specific position.
	bool Remove(unsigned int index);

	/**
	 * Sorts the vector in place, using the given optional
	 * comparison function.
	 * @param cmp_func  Comparison function for vector elements.
	 */
	void Sort(Func* cmp_func = nullptr);
	// void Sort(bool cmp_func(const ValPtr& a, const ValPtr& b));

	/**
	 * Returns a "vector of count" holding the indices of this
	 * vector when sorted using the given (optional) comparison function.
	 * @param cmp_func  Comparison function for vector elements.
	 */
	VectorValPtr Order(Func* cmp_func = nullptr);

	ValPtr ValAt(unsigned int index) const	{ return At(index); }
	RecordVal* RecordValAt(unsigned int index) const
		{ return At(index)->AsRecordVal(); }
	bool BoolAt(unsigned int index) const
		{ return ! At(index)->IsZero(); }
	StringVal* StringValAt(unsigned int index) const
		{ return At(index)->AsStringVal(); }
	const String* StringAt(unsigned int index) const
		{ return StringValAt(index)->AsString(); }

protected:
	/**
	 * Returns the element at a given index or nullptr if it does not exist.
	 * @param index  The position in the vector of the element to return.
	 * @return  The element at the given index or nullptr if the index
	 * does not exist (it's greater than or equal to vector's current size).
	 */
	ValPtr At(unsigned int index) const;

	void ValDescribe(ODesc* d) const override;
	ValPtr DoClone(CloneState* state) override;

private:
	std::vector<ValPtr>* vector_val;
};

#define UNDERLYING_ACCESSOR_DEF(ztype, ctype, name) \
	inline ctype Val::name() const \
		{ return static_cast<const ztype*>(this)->Get(); }

UNDERLYING_ACCESSOR_DEF(detail::IntValImplementation, bro_int_t, AsInt)
UNDERLYING_ACCESSOR_DEF(BoolVal, bool, AsBool)
UNDERLYING_ACCESSOR_DEF(EnumVal, int, AsEnum)
UNDERLYING_ACCESSOR_DEF(detail::UnsignedValImplementation, bro_uint_t, AsCount)
UNDERLYING_ACCESSOR_DEF(detail::DoubleValImplementation, double, AsDouble)
UNDERLYING_ACCESSOR_DEF(TimeVal, double, AsTime)
UNDERLYING_ACCESSOR_DEF(IntervalVal, double, AsInterval)
UNDERLYING_ACCESSOR_DEF(SubNetVal, const IPPrefix&, AsSubNet)
UNDERLYING_ACCESSOR_DEF(AddrVal, const IPAddr&, AsAddr)
UNDERLYING_ACCESSOR_DEF(StringVal, const String*, AsString)
UNDERLYING_ACCESSOR_DEF(FuncVal, Func*, AsFunc)
UNDERLYING_ACCESSOR_DEF(FileVal, File*, AsFile)
UNDERLYING_ACCESSOR_DEF(PatternVal, const RE_Matcher*, AsPattern)
UNDERLYING_ACCESSOR_DEF(TableVal, const PDict<TableEntryVal>*, AsTable)
UNDERLYING_ACCESSOR_DEF(TypeVal, zeek::Type*, AsType)


// Checks the given value for consistency with the given type.  If an
// exact match, returns it.  If promotable, returns the promoted version.
// If not a match, generates an error message and return nil.  If is_init is
// true, then the checking is done in the context of an initialization.
extern ValPtr check_and_promote(
	ValPtr v, const Type* t, bool is_init,
	const detail::Location* expr_location = nullptr);

extern bool same_val(const Val* v1, const Val* v2);
extern bool same_atomic_val(const Val* v1, const Val* v2);
extern bool is_atomic_val(const Val* v);
extern void describe_vals(const ValPList* vals, ODesc* d, int offset=0);
extern void describe_vals(const std::vector<ValPtr>& vals,
                          ODesc* d, size_t offset = 0);
extern void delete_vals(ValPList* vals);

// True if the given Val* has a vector type.
inline bool is_vector(Val* v)	{ return v->GetType()->Tag() == TYPE_VECTOR; }
inline bool is_vector(const ValPtr& v)	{ return is_vector(v.get()); }

// Returns v casted to type T if the type supports that. Returns null if not.
//
// Note: This implements the script-level cast operator.
extern ValPtr cast_value_to_type(Val* v, Type* t);

// Returns true if v can be casted to type T. If so, check_and_cast() will
// succeed as well.
//
// Note: This implements the script-level type comparision operator.
extern bool can_cast_value_to_type(const Val* v, Type* t);

// Returns true if values of type s may support casting to type t. This is
// purely static check to weed out cases early on that will never succeed.
// However, even this function returns true, casting may still fail for a
// specific instance later.
extern bool can_cast_value_to_type(const Type* s, Type* t);

} // namespace zeek

using Val [[deprecated("Remove in v4.1. Use zeek::Val instead.")]] = zeek::Val;
using PortVal [[deprecated("Remove in v4.1. Use zeek::PortVal instead.")]] = zeek::PortVal;
using AddrVal [[deprecated("Remove in v4.1. Use zeek::AddrVal instead.")]] = zeek::AddrVal;
using SubNetVal [[deprecated("Remove in v4.1. Use zeek::SubNetVal instead.")]] = zeek::SubNetVal;
using PatternVal [[deprecated("Remove in v4.1. Use zeek::PatternVal instead.")]] = zeek::PatternVal;
using TableVal [[deprecated("Remove in v4.1. Use zeek::TableVal instead.")]] = zeek::TableVal;
using TableValTimer [[deprecated("Remove in v4.1. Use zeek::TableVal instead.")]] = zeek::TableValTimer;
using RecordVal [[deprecated("Remove in v4.1. Use zeek::RecordVal instead.")]] = zeek::RecordVal;
using ListVal [[deprecated("Remove in v4.1. Use zeek::ListVal instead.")]] = zeek::ListVal;
using StringVal [[deprecated("Remove in v4.1. Use zeek::StringVal instead.")]] = zeek::StringVal;
using EnumVal [[deprecated("Remove in v4.1. Use zeek::EnumVal instead.")]] = zeek::EnumVal;
using VectorVal [[deprecated("Remove in v4.1. Use zeek::VectorVal instead.")]] = zeek::VectorVal;
using TableEntryVal [[deprecated("Remove in v4.1. Use zeek::TableEntryVal instead.")]] = zeek::TableEntryVal;
using TimeVal [[deprecated("Remove in v4.1. Use zeek::TimeVal instead.")]] = zeek::TimeVal;
using DoubleVal [[deprecated("Remove in v4.1. Use zeek::DoubleVal instead.")]] = zeek::DoubleVal;
using IntervalVal [[deprecated("Remove in v4.1. Use zeek::IntervalVal instead.")]] = zeek::IntervalVal;
using ValManager [[deprecated("Remove in v4.1. Use zeek::ValManager instead.")]] = zeek::ValManager;

// Alias for zeek::val_mgr.
extern zeek::ValManager*& val_mgr [[deprecated("Remove in v4.1. Use zeek::val_mgr instead.")]];
