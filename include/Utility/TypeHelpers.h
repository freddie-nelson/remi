namespace blz
{
    using TypeId = unsigned long long;

    /**
     * Generates a unique id for a type.
     *
     * @param typeName The name of the type (use `typeid(T).name()`)
     *
     * @returns The type's id
     */
    TypeId generateTypeId(const char *typeName);

    class TypeIdGenerator
    {
    public:
        /**
         * Generates a unique id using the hash code of the type.
         *
         * NOTE: Switched to using the hash code of the type as the id instead of incrementing a static variable.
         *       This is because different dlls and exes will generate their own version of the template and
         *       therefore have different ids for the same component.
         */
        template <typename T>
        inline static const TypeId id = generateTypeId(typeid(T).name());
    };
}