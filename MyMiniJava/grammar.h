#pragma once

class IVisitor;

class CCodeInfo {
public:
	CCodeInfo( int _lineNumber ) : firstLine( _lineNumber ) { }
	CCodeInfo( int _lineNumber, int _posLine, int _lLine, int _lColomn ) 
		: firstLine( _lineNumber ), firstColumn( _posLine ), lastLine( _lLine ), lastColumn( _lColomn ) { }

	int GetFirstLine() const { return firstLine; }
	int GetFirstColumn() const { return firstColumn; }
	int GetLastLine() const { return lastColumn; }
	int GetLastColumn() const { return lastColumn; }

private:
  int firstLine;
  int firstColumn;
  int lastLine;
  int lastColumn;
};

// �������� ��������� ���������
class IProgram {
public:
	virtual ~IProgram() {}
	virtual void Accept( IVisitor* ) const = 0;
};

// ������� �����
class IMainClass {
public:
	virtual ~IMainClass() {}
	virtual void Accept( IVisitor* ) const = 0;
};

// ������ �������
class IClassDeclList {
public:
	virtual ~IClassDeclList() {}
	virtual void Accept( IVisitor* ) const = 0;
};

// �������� ������
class IClassDecl {
public:
	virtual ~IClassDecl() {}
	virtual void Accept( IVisitor* ) const = 0;
};

// ������ ���������� ����������
class IVarDeclList {
public:
	virtual ~IVarDeclList() {}
	virtual void Accept( IVisitor* ) const = 0;
};

// ���������� ����������
class IVarDecl {
public:
	virtual ~IVarDecl() {}
	virtual void Accept( IVisitor* ) const = 0;
};

// ������ ������� ������
class IMethodDeclList {
public:
	virtual ~IMethodDeclList() {}
	virtual void Accept( IVisitor* ) const = 0;
};

// ����� ������
class IMethodDecl {
public:
	virtual ~IMethodDecl() {}
	virtual void Accept( IVisitor* ) const = 0;
};

// ������ ����������
class IFormalList {
public:
	virtual ~IFormalList() {}
	virtual void Accept( IVisitor* ) const = 0;
};

// ���
class IType {
public:
	virtual ~IType() {}
	virtual void Accept( IVisitor* ) const = 0;
};

// ������������������ ������
class IStmtList {
public:
	virtual ~IStmtList() {}
	virtual void Accept( IVisitor* ) const = 0;
};

// ���� ������� (��� ����)
class IStmt {
public:
	virtual ~IStmt() {}
	virtual void Accept( IVisitor* ) const = 0;
};

// ������������������ ��������� (��������� �������)
class IExprList {
public:
	virtual ~IExprList() {}
	virtual void Accept( IVisitor* ) const = 0;
};

// �������������� ���������
class IExpr {
public:
	virtual ~IExpr() {}
	virtual void Accept( IVisitor* ) const = 0;
};