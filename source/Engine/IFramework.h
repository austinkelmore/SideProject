
class IFramework
{
public:
	IFramework() {}
	virtual ~IFramework() {}

	// functions that need to be overwritten
	virtual bool Init() = 0;
	virtual void Destroy() = 0;
	virtual void Update() = 0;
};