#include "IntermidRepresent.h"
#include "grammar.h"


namespace Translate {

class ISubtreeWrapper {
public:
	virtual ~ISubtreeWrapper() { };
	virtual const IExpr* ToExp() const = 0; // ��� Expr
	virtual const IStmt* ToStm() const = 0; // ��� Stm
	virtual const IStmt* ToConditional( const Temp::CLabel* t, const Temp::CLabel* f ) const = 0; // ��� if/jump
};

}