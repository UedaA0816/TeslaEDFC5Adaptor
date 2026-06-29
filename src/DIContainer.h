//-----------------------------------------------------
//! @file DIContainer.h
//! @brief DIコンテナの実装
//-----------------------------------------------------
#pragma once
#include <functional>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "shared/ILifeCycle.h"
//-----------------------------------------------------
//! @enum Lifecycle
//! @brief サービスのライフサイクル種別
//-----------------------------------------------------
enum class Lifecycle { Transient, Singleton };

//-----------------------------------------------------
//! @brief DIコンテナクラス
//-----------------------------------------------------
class DIContainer {
private:
    using ServiceFactory = std::function<std::shared_ptr<void>()>;

    std::unordered_map<const void*, std::shared_ptr<void>> singletons_; ///< Singleton管理用マップ
    std::unordered_map<const void*, Lifecycle> lifecycles_;             ///< 登録時のライフサイクル
    std::unordered_map<const void*, ServiceFactory> factories_;         ///< 登録済みファクトリ
    std::vector<ILifeCycle*> components_;                               ///< ILifeCycle登録順リスト

    template<typename T>
    static const void* TypeKey() {
        static const char marker = 0;
        return &marker;
    }

    template<typename TService>
    void RegisterInternal(Lifecycle life, const ServiceFactory& factory) {
        const void* serviceType = TypeKey<TService>();

        lifecycles_[serviceType] = life;
        factories_[serviceType] = factory;

        if (life == Lifecycle::Singleton) {
            singletons_[serviceType] = factory();
        }
    }

public:
    //-----------------------------------------------------
    //! @brief サービスを登録する
    //! @tparam T 登録する型
    //! @param life ライフサイクル（Transient / Singleton）
    //-----------------------------------------------------
    template<typename TService, typename TImplementation = TService>
    void Register(Lifecycle life = Lifecycle::Transient) {
        RegisterInternal<TService>(life, []() {
            return std::static_pointer_cast<void>(std::make_shared<TImplementation>());
        });
    }

    template<typename TService>
    void Register(Lifecycle life, std::function<std::shared_ptr<TService>()> factory) {
        RegisterInternal<TService>(life, [factory]() {
            return std::static_pointer_cast<void>(factory());
        });
    }

    //-----------------------------------------------------
    //! @brief ILifeCycle コンポーネントとして登録する（自動収集）
    //! @details 登録順に components() へ追加される。begin/loop 駆動順を制御できる。
    //-----------------------------------------------------
    template<typename TService, typename TImplementation = TService>
    void AddComponent() {
        static_assert(std::is_base_of<ILifeCycle, TService>::value,
                      "AddComponent: TService must implement ILifeCycle");
        Register<TService, TImplementation>(Lifecycle::Singleton);
        components_.push_back(static_cast<ILifeCycle*>(Resolve<TService>().get()));
    }

    template<typename TService>
    void AddComponent(std::function<std::shared_ptr<TService>()> factory) {
        static_assert(std::is_base_of<ILifeCycle, TService>::value,
                      "AddComponent: TService must implement ILifeCycle");
        Register<TService>(Lifecycle::Singleton, factory);
        components_.push_back(static_cast<ILifeCycle*>(Resolve<TService>().get()));
    }

    //-----------------------------------------------------
    //! @brief 登録順の ILifeCycle コンポーネント一覧を返す
    //-----------------------------------------------------
    const std::vector<ILifeCycle*>& components() const { return components_; }

    //-----------------------------------------------------
    //! @brief サービスを解決（インスタンスを取得）する
    //! @tparam T 解決する型
    //! @return インスタンス（ライフサイクルに応じて生成/共有）
    //-----------------------------------------------------
    template<typename T>
    std::shared_ptr<T> Resolve() {
        const void* serviceType = TypeKey<T>();
        auto it = lifecycles_.find(serviceType);

        if (it != lifecycles_.end()) {
            if (it->second == Lifecycle::Singleton) {
                auto singletonIt = singletons_.find(serviceType);
                if (singletonIt != singletons_.end()) {
                    return std::static_pointer_cast<T>(singletonIt->second);
                }
            }

            auto factoryIt = factories_.find(serviceType);
            if (factoryIt != factories_.end()) {
                return std::static_pointer_cast<T>(factoryIt->second());
            }
        }

        return std::shared_ptr<T>();
    }
};
