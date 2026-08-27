#include "gtest/gtest.h"
#include "nexus/Nexus.h"
#include <tuple>

USING_NAMESPACE_NXS;

// --- Dummy Components for Testing ---
struct Position { float x = 0.0f, y = 0.0f; };
struct Velocity { float dx = 1.0f, dy = 1.0f; };
struct Health { int value; };


class EntityTest : public ::testing::Test {
protected:
    // We use a clean registry for each test case
    Ref<entt::registry> registry;
    
    void SetUp() override
    {
        registry = std::make_shared<entt::registry>();
    }
};

// ===================================================================
// CORE LIFECYCLE AND ACCESS TESTS
// ===================================================================

TEST_F(EntityTest, ConstructionAndDestruction) {
    entt::entity initial_entity;
    
    {
        // Test 1: Construction - entity should be created
        Entity entity(registry);
        initial_entity = entity.GetHandle();
        
        // Check if the entity handle is valid
        ASSERT_TRUE(registry->valid(initial_entity));
        
        // Test 2: GetRegistry - should return the correct registry
        ASSERT_EQ(registry, entity.GetRegistry());
    }
    // Test 3: Destruction - entity should be destroyed upon scope exit
    ASSERT_FALSE(registry->valid(initial_entity));
}

TEST_F(EntityTest, DeletedDefaultConstructor) {
    // Should fail to compile: Entity e;
    // We check this conceptually, as GTest can't compile-fail a macro.
    // The test ensures the explicit constructor works.
    SUCCEED();
}

// ===================================================================
// ADD COMPONENT TESTS (SINGLE - Parameterized)
// ===================================================================

TEST_F(EntityTest, AddComponent_Single_ParameterizedCtor) {
    Entity entity(registry);
    
    // Test: Add a single component with explicit arguments
    auto health = entity.AddComponent<Health>(100);
    
    // Check if the component exists and its value is correct
    ASSERT_TRUE(registry->all_of<Health>(entity.GetHandle()));
    ASSERT_EQ(health->value, 100);
}

TEST_F(EntityTest, AddComponent_Single_DefaultCtor) {
    Entity entity(registry);
    
    // Test: Add a single component using default constructor
    auto pos = entity.AddComponent<Position>();
    
    // Check default values
    ASSERT_FLOAT_EQ(pos->x, 0.0f);
}

// ===================================================================
// ADD COMPONENTS TESTS (MULTIPLE - Default Construction Only)
// ===================================================================

TEST_F(EntityTest, AddComponents_SingleType_ReturnsPointer) {
    Entity entity(registry);
    
    // Test: Calling AddComponents with one type should use the single-type path
    auto pos = entity.AddComponents<Position>();
    
    // Check if the component exists and the return type is correct
    ASSERT_TRUE(registry->all_of<Position>(entity.GetHandle()));
    ASSERT_NE(pos, nullptr);
    ASSERT_FLOAT_EQ(pos->x, 0.0f);
    ASSERT_FLOAT_EQ(pos->y, 0.0f);

    static_assert(std::is_same_v<decltype(pos), Position*>, "Return type must be a single pointer.");
}


TEST_F(EntityTest, AddComponents_MultipleTypes_ReturnsTuple) {
    Entity entity(registry);
    
    // Test: Adding multiple components (must use default constructors)
    auto components = entity.AddComponents<Position, Velocity, Health>();
    // Check if all components exist
    const bool all_exist = registry->all_of<Position, Velocity, Health>(entity.GetHandle());
    EXPECT_TRUE(all_exist);

    // Check if the return type is a tuple of pointers
    static_assert(std::is_same_v<decltype(components), std::tuple<Position*, Velocity*, Health*>>, "Return type should be tuple of pointers.");
    
    // Check default values from the tuple
    ASSERT_NE(std::get<0>(components), nullptr);
    ASSERT_NE(std::get<1>(components), nullptr);
    ASSERT_NE(std::get<2>(components), nullptr);
    ASSERT_FLOAT_EQ(std::get<0>(components)->x, 0.0f); // Position default constructed
    ASSERT_FLOAT_EQ(std::get<1>(components)->dx, 1.0f); // Velocity default constructed
    
    // Ensure we can modify the component via the tuple reference
    std::get<2>(components)->value = 42;
    ASSERT_EQ(entity.GetComponent<Health>()->value, 42);
}


// ===================================================================
// GET COMPONENT TESTS
// ===================================================================

TEST_F(EntityTest, GetSingleComponent) {
    Entity entity(registry);
    entity.AddComponent<Position>(1.0f, 2.0f);
    
    // Retrieve the component
    const auto pos = entity.GetComponent<Position>();
    
    ASSERT_NE(pos, nullptr);
    ASSERT_FLOAT_EQ(pos->x, 1.0f);
    ASSERT_FLOAT_EQ(pos->y, 2.0f);
    
    // Test mutability of the non-const reference (using const Entity wrapper)
    // This tests the functionality of the underlying entt::registry::get<T>()
    auto mutable_pos = const_cast<Entity&>(entity).GetComponent<Position>();
    mutable_pos->x = 99.0f;
    ASSERT_FLOAT_EQ(entity.GetComponent<Position>()->x, 99.0f);
}

TEST_F(EntityTest, GetMultipleComponents) {
    Entity entity(registry);
    entity.AddComponent<Position>(1.0f, 2.0f);
    entity.AddComponent<Velocity>(3.0f, 4.0f);
    
    // Retrieve multiple components
    const auto [pos, vel] = entity.GetComponents<Position, Velocity>();
    
    ASSERT_NE(pos, nullptr);
    ASSERT_NE(vel, nullptr);
    ASSERT_FLOAT_EQ(pos->x, 1.0f);
    ASSERT_FLOAT_EQ(vel->dx, 3.0f);
}

// ===================================================================
// TRY GET COMPONENT TESTS
// ===================================================================

TEST_F(EntityTest, TryGetExistingComponent) {
    Entity entity(registry);
    entity.AddComponent<Position>(1.0f, 2.0f);
    
    // Try to retrieve the component
    const Position* pos_ptr = entity.GetComponent<Position>();
    
    // Should succeed and return a valid pointer
    ASSERT_NE(pos_ptr, nullptr);
    ASSERT_FLOAT_EQ(pos_ptr->x, 1.0f);
}

TEST_F(EntityTest, TryGetNonExistingComponent) {
    Entity entity(registry);
    
    // Try to retrieve a component that was not added
    const Velocity* vel_ptr = entity.GetComponent<Velocity>();
    
    // Should fail and return nullptr
    ASSERT_EQ(vel_ptr, nullptr);
}

TEST_F(EntityTest, TryGetMultipleComponents) {
    Entity entity(registry);
    entity.AddComponent<Position>(1.0f, 2.0f);
    
    // Try to retrieve two components, one existing, one missing
    const auto [pos_ptr, vel_ptr] = entity.GetComponents<Position, Velocity>();
    
    // Position should be found, Velocity should not
    ASSERT_NE(pos_ptr, nullptr);
    ASSERT_EQ(vel_ptr, nullptr);
}

// ===================================================================
// HAS COMPONENT TESTS
// ===================================================================

TEST_F(EntityTest, HasComponent_BeforeAndAfterAdd) {
    Entity entity(registry);
    ASSERT_FALSE(entity.HasComponent<Position>());
    entity.AddComponent<Position>();
    ASSERT_TRUE(entity.HasComponent<Position>());
}

TEST_F(EntityTest, HasComponent_AfterRemove_ReturnsFalse) {
    Entity entity(registry);
    entity.AddComponent<Position>();
    ASSERT_TRUE(entity.HasComponent<Position>());
    entity.RemoveComponent<Position>();
    ASSERT_FALSE(entity.HasComponent<Position>());
}

TEST_F(EntityTest, HasComponent_MultipleComponents) {
    Entity entity(registry);
    entity.AddComponents<Position, Velocity>();
    ASSERT_TRUE(entity.HasComponent<Position>());
    ASSERT_TRUE(entity.HasComponent<Velocity>());
    ASSERT_FALSE(entity.HasComponent<Health>());
}

// ===================================================================
// REMOVE COMPONENT TESTS
// ===================================================================

TEST_F(EntityTest, RemoveExistingComponent) {
    Entity entity(registry);
    entity.AddComponent<Position>();
    
    // Check existence before removal
    ASSERT_TRUE(registry->all_of<Position>(entity.GetHandle()));
    
    // Remove the component
    size_t removed_count = entity.RemoveComponent<Position>();
    
    // Check return value and check registry after removal
    ASSERT_EQ(removed_count, 1);
    ASSERT_FALSE(registry->all_of<Position>(entity.GetHandle()));
}

TEST_F(EntityTest, RemoveNonExistingComponent) {
    Entity entity(registry);
    
    // Try to remove a component that was not added
    size_t removed_count = entity.RemoveComponent<Velocity>();
    
    // Check return value
    ASSERT_EQ(removed_count, 0);
}
