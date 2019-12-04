/***************************************************
 *                SPECIALIZATIONS                  *
 ***************************************************/

struct I1 : public MetaDataType<glm::f64> {
    I1() = default;

    explicit I1(const std::vector<double> &data, TensorFeature type) : MetaDataType(data, type){};

    I1* clone() const final {
        return new I1(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("I1"); }
};

struct I2 : MetaDataType<glm::f64> {
    I2() = default;

    explicit I2(const std::vector<double> &data, TensorFeature type) : MetaDataType(data, type){};

    I2* clone() const final {
        return new I2(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("I2"); }
};

struct I3 : MetaDataType<glm::f64> {
    I3() = default;

    explicit I3(const std::vector<double> &data, TensorFeature type) : MetaDataType(data, type){};

    I3* clone() const final {
        return new I3(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("I3"); }
};

struct J1 : MetaDataType<glm::f64> {
    J1() = default;

    explicit J1(const std::vector<double> &data, TensorFeature type) : MetaDataType(data, type){};

    J1* clone() const final {
        return new J1(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("J1"); }
};

struct J2 : MetaDataType<glm::f64> {
    J2() = default;

    explicit J2(const std::vector<double> &data, TensorFeature type) : MetaDataType(data, type){};

    J2* clone() const final {
        return new J2(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("J2"); }
};

struct J3 : MetaDataType<glm::f64> {
    J3() = default;

    explicit J3(const std::vector<double> &data, TensorFeature type) : MetaDataType(data, type){};

    J3* clone() const final {
        return new J3(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("J3"); }
};

struct MajorEigenVectors : MetaDataType<dvec3> {
    MajorEigenVectors() = default;

    explicit MajorEigenVectors(const std::vector<dvec3> &data, TensorFeature type)
        : MetaDataType(data, type){};

    MajorEigenVectors* clone() const final {
        return new MajorEigenVectors(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("MajorEigenVector"); }
};

struct IntermediateEigenVectors : MetaDataType<dvec3> {
    IntermediateEigenVectors() = default;

    explicit IntermediateEigenVectors(const std::vector<dvec3> &data, TensorFeature type)
        : MetaDataType(data, type){};

    IntermediateEigenVectors* clone() const final {
        return new IntermediateEigenVectors(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("IntermediateEigenVector"); }
};

struct MinorEigenVectors : MetaDataType<dvec3> {
    MinorEigenVectors() = default;

    explicit MinorEigenVectors(const std::vector<dvec3> &data, TensorFeature type)
        : MetaDataType(data, type){};

    MinorEigenVectors* clone() const final {
        return new MinorEigenVectors(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("MinorEigenVector"); }
};

struct MajorEigenValues : MetaDataType<glm::f64> {
    MajorEigenValues() = default;

    explicit MajorEigenValues(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    MajorEigenValues* clone() const final {
        return new MajorEigenValues(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("Sigma1"); }
};

struct IntermediateEigenValues : MetaDataType<glm::f64> {
    IntermediateEigenValues() = default;

    explicit IntermediateEigenValues(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    IntermediateEigenValues* clone() const final {
        return new IntermediateEigenValues(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("Sigma2"); }
};

struct MinorEigenValues : MetaDataType<glm::f64> {
    MinorEigenValues() = default;

    explicit MinorEigenValues(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    MinorEigenValues* clone() const final {
        return new MinorEigenValues(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("Sigma3"); }
};

struct LodeAngle : MetaDataType<glm::f64> {
    LodeAngle() = default;

    explicit LodeAngle(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    LodeAngle* clone() const final {
        return new LodeAngle(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("LodeAngle"); }
};

struct Anisotropy : MetaDataType<glm::f64> {
    Anisotropy() = default;

    explicit Anisotropy(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    Anisotropy* clone() const final {
        return new Anisotropy(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("Anisotropy"); }
};

struct LinearAnisotropy : MetaDataType<glm::f64> {
    LinearAnisotropy() = default;

    explicit LinearAnisotropy(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    LinearAnisotropy* clone() const final {
        return new LinearAnisotropy(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("LinearAnisotropy"); }
};

struct PlanarAnisotropy : MetaDataType<glm::f64> {
    PlanarAnisotropy() = default;

    explicit PlanarAnisotropy(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    PlanarAnisotropy* clone() const final {
        return new PlanarAnisotropy(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("PlanarAnisotropy"); }
};

struct SphericalAnisotropy : MetaDataType<glm::f64> {
    SphericalAnisotropy() = default;

    explicit SphericalAnisotropy(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    SphericalAnisotropy* clone() const final {
        return new SphericalAnisotropy(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("SphericalAnisotropy"); }
};

struct Diffusivity : MetaDataType<glm::f64> {
    Diffusivity() = default;

    explicit Diffusivity(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    Diffusivity* clone() const final {
        return new Diffusivity(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("Diffusivity"); }
};

struct ShearStress : MetaDataType<glm::f64> {
    ShearStress() = default;

    explicit ShearStress(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    ShearStress* clone() const final {
        return new ShearStress(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("ShearStress"); }
};

struct PureShear : MetaDataType<glm::f64> {
    PureShear() = default;

    explicit PureShear(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    PureShear* clone() const final {
        return new PureShear(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("PureShear"); }
};

struct ShapeFactor : MetaDataType<glm::f64> {
    ShapeFactor() = default;

    explicit ShapeFactor(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    ShapeFactor* clone() const final {
        return new ShapeFactor(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("ShapeFactor"); }
};

struct IsotropicScaling : MetaDataType<glm::f64> {
    IsotropicScaling() = default;

    explicit IsotropicScaling(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    IsotropicScaling* clone() const final {
        return new IsotropicScaling(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("IsotropicScaling"); }
};

struct Rotation : MetaDataType<glm::f64> {
    Rotation() = default;

    explicit Rotation(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    Rotation* clone() const final {
        return new Rotation(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("Rotation"); }
};

struct FrobeniusNorm : MetaDataType<glm::f64> {
    FrobeniusNorm() = default;

    explicit FrobeniusNorm(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    FrobeniusNorm* clone() const final {
        return new FrobeniusNorm(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("FrobeniusNorm"); }
};

struct HillYieldCriterion : MetaDataType<glm::f64> {
    HillYieldCriterion() = default;

    explicit HillYieldCriterion(const std::vector<double> &data, TensorFeature type)
        : MetaDataType(data, type){};

    HillYieldCriterion* clone() const final {
        return new HillYieldCriterion(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("HillYieldCriterion"); }
};
