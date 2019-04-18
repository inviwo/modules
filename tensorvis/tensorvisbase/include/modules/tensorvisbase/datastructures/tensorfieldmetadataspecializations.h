/***************************************************
 *                SPECIALIZATIONS                  *
 ***************************************************/

struct I1 : public MetaDataType<glm::f64> {
    I1() = default;

    explicit I1(const std::vector<double> &data, TensorFeature type) : MetaDataType(data, type){};

    std::unique_ptr<MetaDataBase> clone() const override {
        return std::make_unique<I1>(data_, type_);
    }

    uint64_t getId() const override { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("I1"); }
};

struct I2 : MetaDataType<glm::f64> {
    I2() = default;

    explicit I2(const std::vector<double> &data, TensorFeature type) : MetaDataType(data, type){};

    std::unique_ptr<MetaDataBase> clone() const override {
        return std::make_unique<I2>(data_, type_);
    }

    uint64_t getId() const override { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("I2"); }
};

struct I3 : MetaDataType<glm::f64> {
    I3() = default;

    explicit I3(const std::vector<double> &data, TensorFeature type) : MetaDataType(data, type){};

    std::unique_ptr<MetaDataBase> clone() const override {
        return std::make_unique<I3>(data_, type_);
    }

    uint64_t getId() const override { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("I3"); }
};

struct J1 : MetaDataType<glm::f64> {
    J1() = default;

    explicit J1(const std::vector<double> &data, TensorFeature type) : MetaDataType(data, type){};

    std::unique_ptr<MetaDataBase> clone() const override {
        return std::make_unique<J1>(data_, type_);
    }

    uint64_t getId() const override { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("J1"); }
};

struct J2 : MetaDataType<glm::f64> {
    J2() = default;

    explicit J2(const std::vector<double> &data, TensorFeature type) : MetaDataType(data, type){};

    std::unique_ptr<MetaDataBase> clone() const override {
        return std::make_unique<J2>(data_, type_);
    }

    uint64_t getId() const override { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("J2"); }
};

struct J3 : MetaDataType<glm::f64> {
    J3() = default;

    explicit J3(const std::vector<double> &data, TensorFeature type) : MetaDataType(data, type){};

    std::unique_ptr<MetaDataBase> clone() const override {
        return std::make_unique<J3>(data_, type_);
    }

    uint64_t getId() const override { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("J3"); }
};

struct MajorEigenVectors : MetaDataType<dvec3> {
    MajorEigenVectors() = default;

    explicit MajorEigenVectors(const std::vector<dvec3> &data, TensorFeature type)
        : MetaDataType(data, type){};

    std::unique_ptr<MetaDataBase> clone() const override {
        return std::make_unique<MajorEigenVectors>(data_, type_);
    }

    uint64_t getId() const override { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("MajorEigenVectors"); }
};

struct IntermediateEigenVectors : MetaDataType<dvec3> {
    IntermediateEigenVectors() = default;

    explicit IntermediateEigenVectors(const std::vector<dvec3> &data, TensorFeature type)
        : MetaDataType(data, type){};

    std::unique_ptr<MetaDataBase> clone() const override {
        return std::make_unique<IntermediateEigenVectors>(data_, type_);
    }

    uint64_t getId() const override { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("IntermediateEigenVectors"); }
};

struct MinorEigenVectors : MetaDataType<dvec3> {
    MinorEigenVectors() = default;

    explicit MinorEigenVectors(const std::vector<dvec3> &data, TensorFeature type)
        : MetaDataType(data, type){};

    std::unique_ptr<MetaDataBase> clone() const override {
        return std::make_unique<MinorEigenVectors>(data_, type_);
    }

    uint64_t getId() const override { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("MinorEigenVectors"); }
};

struct MajorEigenValues : MetaDataType<glm::f64> {
    MajorEigenValues() = default;

    explicit MajorEigenValues(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    std::unique_ptr<MetaDataBase> clone() const override {
        return std::make_unique<MajorEigenValues>(data_, type_);
    }

    uint64_t getId() const override { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("MajorEigenValues"); }
};

struct IntermediateEigenValues : MetaDataType<glm::f64> {
    IntermediateEigenValues() = default;

    explicit IntermediateEigenValues(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    std::unique_ptr<MetaDataBase> clone() const override {
        return std::make_unique<IntermediateEigenValues>(data_, type_);
    }

    uint64_t getId() const override { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("IntermediateEigenValues"); }
};

struct MinorEigenValues : MetaDataType<glm::f64> {
    MinorEigenValues() = default;

    explicit MinorEigenValues(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    std::unique_ptr<MetaDataBase> clone() const override {
        return std::make_unique<MinorEigenValues>(data_, type_);
    }

    uint64_t getId() const override { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("MinorEigenValues"); }
};

struct LodeAngle : MetaDataType<glm::f64> {
    LodeAngle() = default;

    explicit LodeAngle(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    std::unique_ptr<MetaDataBase> clone() const override {
        return std::make_unique<LodeAngle>(data_, type_);
    }

    uint64_t getId() const override { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("LodeAngle"); }
};

struct Anisotropy : MetaDataType<glm::f64> {
    Anisotropy() = default;

    explicit Anisotropy(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    std::unique_ptr<MetaDataBase> clone() const override {
        return std::make_unique<Anisotropy>(data_, type_);
    }

    uint64_t getId() const override { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("Anisotropy"); }
};

struct LinearAnisotropy : MetaDataType<glm::f64> {
    LinearAnisotropy() = default;

    explicit LinearAnisotropy(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    std::unique_ptr<MetaDataBase> clone() const override {
        return std::make_unique<LinearAnisotropy>(data_, type_);
    }

    uint64_t getId() const override { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("LinearAnisotropy"); }
};

struct PlanarAnisotropy : MetaDataType<glm::f64> {
    PlanarAnisotropy() = default;

    explicit PlanarAnisotropy(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    std::unique_ptr<MetaDataBase> clone() const override {
        return std::make_unique<PlanarAnisotropy>(data_, type_);
    }

    uint64_t getId() const override { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("PlanarAnisotropy"); }
};

struct SphericalAnisotropy : MetaDataType<glm::f64> {
    SphericalAnisotropy() = default;

    explicit SphericalAnisotropy(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    std::unique_ptr<MetaDataBase> clone() const override {
        return std::make_unique<SphericalAnisotropy>(data_, type_);
    }

    uint64_t getId() const override { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("SphericalAnisotropy"); }
};

struct Diffusivity : MetaDataType<glm::f64> {
    Diffusivity() = default;

    explicit Diffusivity(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    std::unique_ptr<MetaDataBase> clone() const override {
        return std::make_unique<Diffusivity>(data_, type_);
    }

    uint64_t getId() const override { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("Diffusivity"); }
};

struct ShearStress : MetaDataType<glm::f64> {
    ShearStress() = default;

    explicit ShearStress(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    std::unique_ptr<MetaDataBase> clone() const override {
        return std::make_unique<ShearStress>(data_, type_);
    }

    uint64_t getId() const override { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("ShearStress"); }
};

struct PureShear : MetaDataType<glm::f64> {
    PureShear() = default;

    explicit PureShear(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    std::unique_ptr<MetaDataBase> clone() const override {
        return std::make_unique<PureShear>(data_, type_);
    }

    uint64_t getId() const override { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("PureShear"); }
};

struct ShapeFactor : MetaDataType<glm::f64> {
    ShapeFactor() = default;

    explicit ShapeFactor(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    std::unique_ptr<MetaDataBase> clone() const override {
        return std::make_unique<ShapeFactor>(data_, type_);
    }

    uint64_t getId() const override { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("ShapeFactor"); }
};

struct IsotropicScaling : MetaDataType<glm::f64> {
    IsotropicScaling() = default;

    explicit IsotropicScaling(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    std::unique_ptr<MetaDataBase> clone() const override {
        return std::make_unique<IsotropicScaling>(data_, type_);
    }

    uint64_t getId() const override { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("IsotropicScaling"); }
};

struct Rotation : MetaDataType<glm::f64> {
    Rotation() = default;

    explicit Rotation(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    std::unique_ptr<MetaDataBase> clone() const override {
        return std::make_unique<Rotation>(data_, type_);
    }

    uint64_t getId() const override { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("Rotation"); }
};

struct FrobeniusNorm : MetaDataType<glm::f64> {
    FrobeniusNorm() = default;

    explicit FrobeniusNorm(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    std::unique_ptr<MetaDataBase> clone() const override {
        return std::make_unique<FrobeniusNorm>(data_, type_);
    }

    uint64_t getId() const override { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("FrobeniusNorm"); }
};

struct HillYieldCriterion : MetaDataType<glm::f64> {
    HillYieldCriterion() = default;

    explicit HillYieldCriterion(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    std::unique_ptr<MetaDataBase> clone() const override {
        return std::make_unique<HillYieldCriterion>(data_, type_);
    }

    uint64_t getId() const override { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("HillYieldCriterion"); }
};
