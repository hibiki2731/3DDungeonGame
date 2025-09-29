#pragma once
#include <memory>
#include <vector>
#include <concepts>
#include <algorithm>

//前方宣言
class Actor;

class Component : public std::enable_shared_from_this<Component>
{
public:
	Component();
	virtual ~Component();

	//初期化
	void init(const std::shared_ptr<Actor>& owner, int updateOrder = 100);
	virtual void initComponent() {};

	//入力
	virtual void inputComponent() {};

	//更新
	virtual void updateComponent();
	int getUpdateOrder() const { return mUpdateOrder; }

private:
	//newを禁止にする
	static void* operator new(size_t size);

protected:
	//所有アクター
	std::shared_ptr<Actor> mOwner;
	//更新順序
	int mUpdateOrder;

};

//Componentの作成
//テンプレートの制約
template <typename ComponentType> concept isComponent = std::is_base_of<Component, ComponentType>::value == true;
//initを持つクラスの生成関数
template <isComponent ComponentType>
std::shared_ptr<ComponentType> createComponent(const std::shared_ptr<Actor>& owner) {
	std::shared_ptr<ComponentType> component = std::make_shared<ComponentType>();
	component->init(owner);
	component->initComponent();
	return component;
};
