//-----------------------------------------------------
//! @file DIContainer.h
//! @brief DIコンテナの実装（Singleton 専用）
//-----------------------------------------------------
#pragma once
#include <functional>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "shared/ILifeCycle.h"

//-----------------------------------------------------
//! @brief DIコンテナクラス（全サービスは Singleton）
//-----------------------------------------------------
class DIContainer {
private:
    using ServiceFactory = std::function<std::shared_ptr<void>()>;

    std::unordered_map<const void*, std::shared_ptr<void>> singletons_; ///< Singleton 管理用マップ
    std::vector<ILifeCycle*> components_;                               ///< ILifeCycle 登録順リスト

    template<typename T>
    static const void* TypeKey() {
        static const char marker = 0;
        return &marker;
    }

    //-----------------------------------------------------
    //! @brief 内部登録：ファクトリを即時実行して Singleton として保持する
    //-----------------------------------------------------
    template<typename TService>
    void RegisterSingleton(const ServiceFactory& factory) {
        singletons_[TypeKey<TService>()] = factory();
    }

public:
    //-----------------------------------------------------
    //! @brief ILifeCycle コンポーネントとして登録する（自動収集）
    //! @details 登録順に components() へ追加される。begin/loop 駆動順を制御できる。
    //-----------------------------------------------------
    template<typename TService, typename TImplementation = TService>
    void AddComponent() {
        static_assert(std::is_base_of<ILifeCycle, TService>::value,
                      "AddComponent: TService must implement ILifeCycle");
        RegisterSingleton<TService>([]() {
            return std::static_pointer_cast<void>(std::make_shared<TImplementation>());
        });
        components_.push_back(static_cast<ILifeCycle*>(Resolve<TService>().get()));
    }

    template<typename TService>
    void AddComponent(std::function<std::shared_ptr<TService>()> factory) {
        static_assert(std::is_base_of<ILifeCycle, TService>::value,
                      "AddComponent: TService must implement ILifeCycle");
        RegisterSingleton<TService>([factory]() {
            return std::static_pointer_cast<void>(factory());
        });
        components_.push_back(static_cast<ILifeCycle*>(Resolve<TService>().get()));
    }

    //-----------------------------------------------------
    //! @brief 登録順の ILifeCycle コンポーネント一覧を返す
    //-----------------------------------------------------
    const std::vector<ILifeCycle*>& components() const { return components_; }

    //-----------------------------------------------------
    //! @brief 登録済み Singleton を解決して返す
    //! @tparam T 解決する型
    //! @return 登録済みインスタンス（未登録の場合は空の shared_ptr）
    //-----------------------------------------------------
    template<typename T>
    std::shared_ptr<T> Resolve() {
        auto it = singletons_.find(TypeKey<T>());
        if (it != singletons_.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        return std::shared_ptr<T>();
    }
};
