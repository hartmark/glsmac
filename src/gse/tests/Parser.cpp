#include "GSE.h"

#include "gse/Tests.h"
#include "gse/type/Int.h"
#include "gse/program/Program.h"
#include "gse/program/Scope.h"
#include "gse/program/Statement.h"
#include "gse/program/Expression.h"
#include "gse/program/Operator.h"
#include "gse/program/Value.h"
#include "gse/program/Variable.h"
#include "gse/program/Object.h"
#include "gse/program/Function.h"
#include "gse/program/Call.h"
#include "gse/parser/GJS.h"

namespace gse {
namespace tests {

using namespace program;

void AddParserTests( task::gsetests::GSETests* task ) {

	const Program* reference_program = GetTestProgram();

	const auto validate_program = [ reference_program ]( const Program* program ) -> std::string {
		GT_ASSERT( program != nullptr, "parser returned null program" );

#define VALIDATOR( _type, ... ) [ __VA_ARGS__ ]( const _type* a, const _type* b ) -> std::string
#define VALIDATOR_FORWARD_DEF( _var, _type ) std::function< std::string( const _type* a, const _type* b ) > _var;
#define VALIDATE( _func, _a, _b ) \
        errmsg = _func( (_a), (_b) ); \
        if (!errmsg.empty()) return errmsg;

		std::string errmsg = "";

		VALIDATOR_FORWARD_DEF( scope, Scope );
		VALIDATOR_FORWARD_DEF( expression, Expression );
		VALIDATOR_FORWARD_DEF( operand, Operand );

		const auto value = VALIDATOR( program::Value ) {
			GT_ASSERT( a->value == b->value, "values differ ( " + a->value.ToString() + " != " + b->value.ToString() + " )" );
			GT_OK();
		};

		const auto variable = VALIDATOR( Variable ) {
			GT_ASSERT( a->name == b->name, "variables names differ ( \"" + a->name + "\" != \"" + b->name + "\" )" );
			GT_ASSERT( a->hints == b->hints, "variables hints differ ( \"" + std::to_string( a->hints ) + "\" != \"" + std::to_string( b->hints ) + "\" )" );
			GT_OK();
		};

		const auto object = VALIDATOR( Object, &errmsg, &operand ) {
			for ( const auto& it : a->properties ) {
				GT_ASSERT( b->properties.find( it.first ) != b->properties.end(), "property \"" + it.first + "\" exists in A but not B" );
			}
			for ( const auto& it : b->properties ) {
				GT_ASSERT( a->properties.find( it.first ) != a->properties.end(), "property \"" + it.first + "\" exists in B but not A" );
			}
			for ( const auto& it : a->properties ) {
				VALIDATE( operand, it.second, b->properties.at( it.first ) );
			}
			GT_OK();
		};

		const auto function = VALIDATOR( Function, &errmsg, &scope ) {
			GT_ASSERT( a->parameters.size() == b->parameters.size(), "parameters sizes differ ( " + std::to_string( a->parameters.size() ) + " != " + std::to_string( b->parameters.size() ) + " )" );
			VALIDATE( scope, a->body, b->body );
			GT_OK();
		};

		const auto call = VALIDATOR( Call, &errmsg, &expression, &operand ) {
			VALIDATE( expression, a->callable, b->callable );
			GT_ASSERT( a->arguments.size() == b->arguments.size(), "arguments sizes differ ( " + std::to_string( a->arguments.size() ) + " != " + std::to_string( b->arguments.size() ) + " )" );
			for ( size_t i = 0 ; i < a->arguments.size() ; i++ ) {
				VALIDATE( operand, a->arguments[ i ], b->arguments[ i ] );
			}
			GT_OK();
		};

		operand = VALIDATOR( Operand,
			&errmsg,
			&value,
			&variable,
			&object,
			&scope,
			&expression,
			&function,
			&call
		) {
			GT_ASSERT( ( a == nullptr ) == ( b == nullptr ), "operands have different null states ( " + ( a == nullptr
				? "null"
				: "not null"
			) + " != " + ( b == nullptr
				? "null"
				: "not null"
			) + " )" );
			if ( a && b ) {
				GT_ASSERT( a->type == b->type, "operands have different types ( " + std::to_string( a->type ) + " != " + std::to_string( b->type ) + " )" );
				switch ( a->type ) {
					case Operand::OT_NOTHING: {
						break;
					}
					case Operand::OT_VALUE: {
						VALIDATE( value, (program::Value*)a, (program::Value*)b );
						break;
					}
					case Operand::OT_VARIABLE: {
						VALIDATE( variable, (Variable*)a, (Variable*)b );
						break;
					}
					case Operand::OT_OBJECT: {
						VALIDATE( object, (Object*)a, (Object*)b );
						break;
					}
					case Operand::OT_SCOPE: {
						VALIDATE( scope, (Scope*)a, (Scope*)b );
						break;
					}
					case Operand::OT_EXPRESSION: {
						VALIDATE( expression, (Expression*)a, (Expression*)b );
						break;
					}
					case Operand::OT_FUNCTION: {
						VALIDATE( function, (Function*)a, (Function*)b );
						break;
					}
					case Operand::OT_CALL: {
						VALIDATE( call, (Call*)a, (Call*)b );
						break;
					}
					default: {
						GT_ASSERT( false, "unknown operand type: " + std::to_string( a->type ) );
					}
				}
			}
			GT_OK();
		};
		const auto operatr = VALIDATOR( Operator ) {
			GT_ASSERT( ( a == nullptr ) == ( b == nullptr ), "operators have different null states ( " + ( a == nullptr
				? "null"
				: "not null"
			) + " != " + ( b == nullptr
				? "null"
				: "not null"
			) + " )" );
			if ( a && b ) {
				GT_ASSERT( a->op == b->op, "operators are different ( " + std::to_string( a->op ) + " != " + std::to_string( b->op ) + " )" );
			}
			GT_OK();
		};
		expression = VALIDATOR( Expression, &errmsg, &operand, &operatr ) {
			VALIDATE( operand, a->a, b->a );
			VALIDATE( operatr, a->op, b->op );
			VALIDATE( operand, a->b, b->b );
			GT_OK();
		};
		const auto statement = VALIDATOR( Statement, &errmsg, &expression ) {
			VALIDATE( expression, a->body, b->body );
			GT_OK();
		};
		scope = VALIDATOR( Scope, &errmsg, &statement ) {
			GT_ASSERT( a->body.size() == b->body.size(), "scope sizes differ ( " + std::to_string( a->body.size() ) + " != " + std::to_string( b->body.size() ) + " )" );
			for ( size_t i = 0 ; i < a->body.size() ; i++ ) {
				VALIDATE( statement, a->body[ i ], b->body[ i ] );
			}
			GT_OK();
		};

		VALIDATE( scope, reference_program->body, program->body );

#undef VALIDATOR
#undef VALIDATE
		GT_OK();
	};

	task->AddTest(
		"test if GJS parser produces valid output",
		GT( validate_program ) {
			parser::GJS parser(
				""
				"// test script\n"
				"\n"
				"let a = 5;\n"
				"a++;\n"
				"let b = a + 2 * 4;\n"
				"let c=(a+2)*4;\n"
				"{\n"
				"	a = 15;\n"
				"	a += 10;\n"
				"};\n"
				"c = 123;\n"
				"c -= 23;\n"
				"\n"
				"let testmethod1 = (a, b, c) => { return a + b + c; };\n"
				"\n"
				"let testmethod2 = (a, b, c) => {\n"
				"	/*\n"
				"		this method is a bit different\n"
				"	*/\n"
				"	return\n"
				"		a\n"
				"			+\n"
				"		b\n"
				"			-\n"
				"		c\n"
				"	;\n"
				"};\n"
				"\n"
				"let testobj1 = {};\n"
				"let testobj2 = {\n"
				"	propertyString: 'STRING',\n"
				"	propertyInt1: 111 + a + b,\n"
				"	propertyInt2: 222,\n"
				"};\n"
				"let testobj3 = {\n"
				"  child1: {\n"
				"    child2: {\n"
				"      value: 'CHILD VALUE'\n"
				"    }\n"
				"  },\n"
				"};\n"
				"testobj1.propertyInt = testobj2.propertyInt1 + testobj2.propertyInt2;\n"
				"\n"
				"let d = null;\n"
				"let x = a > b;\n"
				"\n"
				"console.log( d );\n"
				"console.log( d == null );\n"
				"console.log( x, x == b > c );\n"
				"\n"
				"console.log( a != b, b != c, c != a, a != a );\n"
				"console.log( a > b, b > c );\n"
				"console.log( b >= a, a >= 2, c <= 200, a <= 200 );\n"
				"console.log( 10 < 10, 10 <= 10, a < a, a <= a );\n"
				"console.log( true && true, true && false, true || true, true || false );\n"
				"console.log( (( 5 > 10 ) && ( 2 > 1 )) || (( 5 <= 10 ) && !( 5 > 35 ) && ( 100 >= 20 )) );\n"
				"console.log(testmethod1(11, b, 20), testmethod2(a, b, c));\n"
				"let testmethod = testmethod1;\n"
				"console.log( testmethod( 1, testmethod( 2, testmethod( 3, 3, 3 ), testmethod( 4, 4, 4 ) ), testmethod( 5, 5, testmethod( 6, 6, 6 )) ), 10 );\n"
				"console.log(testobj3.child1.child2.value);\n"
				"console.log(testobj1.propertyInt == 272 + c); console.log(testobj1, testobj2);\n"
				"\n"
				"console.log('bye!');\n"
			);
			const auto* program = parser.Parse();
			const auto result = validate_program( program );
			if ( program ) {
				DELETE( program );
			}
			return result;
		}
	);

}

}
}
