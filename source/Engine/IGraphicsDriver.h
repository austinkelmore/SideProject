
class IGraphicsDriver
{
public:
	IGraphicsDriver() {}
	~IGraphicsDriver() {}

	virtual bool Init() = 0;
	virtual void Update() = 0;
	virtual void Destroy() = 0;
};