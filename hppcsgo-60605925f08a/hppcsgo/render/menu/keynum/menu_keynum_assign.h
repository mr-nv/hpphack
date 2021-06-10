class CMenuKeynumAssign
{
public:
	CMenuKeynumAssign();
	~CMenuKeynumAssign();

	bool IsKeyAssignment();	
	void KeyAssignment(unsigned code);

protected:
	void KeyAssignmentStart();
	int GetAssigmentKeynum();

private:
	bool m_assignment;
	int m_keynum;
};