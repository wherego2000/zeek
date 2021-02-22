// See the file "COPYING" in the main distribution directory for copyright.

#include "zeek/ZeekString.h"
#include "zeek/File.h"
#include "zeek/Func.h"
#include "zeek/OpaqueVal.h"
#include "zeek/Reporter.h"
#include "zeek/Desc.h"

using namespace zeek;


bool ZVal::zval_error_status = false;


bool zeek::IsManagedType(const TypePtr& t)
	{
	switch ( t->Tag() ) {
	case zeek::TYPE_ADDR:
	case zeek::TYPE_ANY:
	case zeek::TYPE_FILE:
	case zeek::TYPE_FUNC:
	case zeek::TYPE_LIST:
	case zeek::TYPE_OPAQUE:
	case zeek::TYPE_PATTERN:
	case zeek::TYPE_RECORD:
	case zeek::TYPE_STRING:
	case zeek::TYPE_SUBNET:
	case zeek::TYPE_TABLE:
	case zeek::TYPE_TYPE:
	case zeek::TYPE_VECTOR:
		return true;

	default:
		return false;

	}
	}


ZVal::ZVal(ValPtr v, const TypePtr& t)
	{
	if ( ! v )
		{
		// This can happen for some forms of error propagation.
		// We can deal with it iff the type is managed, and thus
		// we can employ a "nil" placeholder.
		ASSERT(IsManagedType(t));
		managed_val = nullptr;
		return;
		}

	auto vt = v->GetType();

	if ( vt->Tag() != t->Tag() && t->Tag() != TYPE_ANY )
		{
		if ( t->InternalType() == TYPE_INTERNAL_OTHER ||
		     t->InternalType() != vt->InternalType() )
			reporter->InternalError("type inconsistency in ZVal constructor");
		}

	switch ( t->Tag() ) {
	case TYPE_BOOL:
	case TYPE_INT:
	case TYPE_ENUM:
		int_val = v->AsInt();
		break;

	case TYPE_COUNT:
	case TYPE_PORT:
		uint_val = v->AsCount();
		break;

	case TYPE_DOUBLE:
	case TYPE_INTERVAL:
	case TYPE_TIME:
		double_val = v->AsDouble();
		break;

	case TYPE_FUNC:
		func_val = v->AsFunc();
		Ref(func_val);
		break;

	case TYPE_FILE:
		file_val = v->AsFile();
		Ref(file_val);
		break;

	case TYPE_LIST:
		list_val = v.release()->AsListVal();
		break;

	case TYPE_OPAQUE:
		opaque_val = v.release()->AsOpaqueVal();
		break;

	case TYPE_PATTERN:
		re_val = v.release()->AsPatternVal();
		break;

	case TYPE_TABLE:
		table_val = v.release()->AsTableVal();
		break;

	case TYPE_VECTOR:
		{
		vector_val = v.release()->AsVectorVal();

		// Some run-time type-checking, sigh.
		auto my_ytag = t->AsVectorType()->Yield()->Tag();
		auto v_ytag = vt->AsVectorType()->Yield()->Tag();

		if ( my_ytag != v_ytag && my_ytag != TYPE_ANY &&
		     v_ytag != TYPE_ANY )
			{
			// Despite the above checks, this clash can still
			// happen thanks to the intercession of vector-of-any,
			// which for example can allow a function to return
			// a concrete vector-of-X that's assigned to a local
			// with a concrete vector-of-Y type.
			reporter->Error("vector type clash: %s vs. %s (%s)",
					type_name(my_ytag), type_name(v_ytag),
					obj_desc(v.get()).c_str());
			zval_error_status = true;
			}

		break;
		}

	case TYPE_RECORD:
		record_val = v.release()->AsRecordVal();
		break;

	case TYPE_STRING:
		string_val = v.release()->AsStringVal();
		break;

	case TYPE_ADDR:
		addr_val = v.release()->AsAddrVal();
		break;

	case TYPE_SUBNET:
		subnet_val = v.release()->AsSubNetVal();
		break;

	case TYPE_ANY:
		any_val = v.release();
		break;

	case TYPE_TYPE:
		type_val = t->Ref();
		break;

	case TYPE_ERROR:
	case TYPE_TIMER:
	case TYPE_UNION:
	case TYPE_VOID:
		reporter->InternalError("bad type in ZVal constructor");
	}
	}

ValPtr ZVal::ToVal(const TypePtr& t) const
	{
	Val* v;

	switch ( t->Tag() ) {
	case TYPE_INT:
		return val_mgr->Int(int_val);

	case TYPE_BOOL:
		return val_mgr->Bool(int_val ? true : false);

	case TYPE_PORT:
		return val_mgr->Port(uint_val);

	case TYPE_COUNT:
		return val_mgr->Count(uint_val);

	case TYPE_DOUBLE:
		return make_intrusive<DoubleVal>(double_val);

	case TYPE_INTERVAL:
		return make_intrusive<IntervalVal>(double_val, Seconds);

	case TYPE_TIME:
		return make_intrusive<TimeVal>(double_val);

	case TYPE_ENUM:
		return t->AsEnumType()->GetEnumVal(int_val);

	case TYPE_ANY:
		return {NewRef{}, any_val};

	case TYPE_TYPE:
		{
		TypePtr tp = {NewRef{}, type_val};
		return make_intrusive<TypeVal>(tp);
		}

	case TYPE_FUNC:
		if ( func_val )
			{
			FuncPtr fv_ptr = {NewRef{}, func_val};
			return make_intrusive<FuncVal>(fv_ptr);
			}

		return nullptr;

	case TYPE_FILE:
		if ( file_val )
			{
			FilePtr fv_ptr = {NewRef{}, file_val};
			return make_intrusive<FileVal>(fv_ptr);
			}

		return nullptr;

	case TYPE_ADDR:		v = addr_val; break;
	case TYPE_SUBNET:	v = subnet_val; break;
	case TYPE_STRING:	v = string_val; break;
	case TYPE_LIST:		v = list_val; break;
	case TYPE_OPAQUE:	v = opaque_val; break;
	case TYPE_TABLE:	v = table_val; break;
	case TYPE_RECORD:	v = record_val; break;
	case TYPE_VECTOR:	v = vector_val; break;
	case TYPE_PATTERN:	v = re_val; break;

	case TYPE_ERROR:
	case TYPE_TIMER:
	case TYPE_UNION:
	case TYPE_VOID:
		reporter->InternalError("bad ret type return tag");
	}

	if ( v )
		return {NewRef{}, v};

	reporter->Error("value used but not set");
	zval_error_status = true;

	return nullptr;
	}
