namespace fm {

class IColumn {
public:
    virtual std::string get_name() const = 0;
    virtual std::string get_definition() const = 0;
    virtual void bind_value(sqlite3_stmt* stmt, int index) const = 0;
    virtual ~IColumn() = default;
};

}