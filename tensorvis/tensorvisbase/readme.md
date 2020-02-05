# TensorVisBase module

This module contains some of the basic tensor visualization techniques.
For loading/saving tensor fields see the
[_tensorvisio_](https://github.com/inviwo/modules/tree/master/tensorvis/tensorvisio) module.

## Functionalities included in this module:
- HyperLIC
- Anisotropy raycasting
- Invariant space based plots (PCP, Scatterplot)

## Available visualizers
### HyperLIC
Available for 2D and 3D tensor fields (3D tensor fields will be sliced).
Based on the standard LIC method, HyperLIC is indicating the directions of
the major/minor eigenvectors.

### Anisotropy raycasting
1. A scalar volume is generated, values are given by |&sigma;<sub>1</sub>-&sigma;<sub>3</sub>|.
2. Raycasting is applied.

## Adding your own meta data
There are several standard meta data available for 2D and 3D tensor fields, such as different anisotropies, the lode angle, the Frobenius Norm, and the I and J invariants.

If you wish to add your own meta data and have it automatically available in Inviwo's processors, do as follows (it's very simple):
1. Add a struct to `attributes.h` containing the following:
    - identifier, i.e.
    ```c++
    static constexpr inline std::string_view("MyMetaData");
    ```

    - A method called `calculate` which defines how to calculate the meta data from either the tensors or the available meta data. The signature of this method must look as follows:
    ```c++
    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData);
    ```

    - The struct needs to inherit from `TypedAttributeBase<T>` where `T` defines the data type of your meta data, e.g. for anisotropy it's `float` and for the eigen vectors it's `vec3`.

2. Implement the `calculate` method. It's up to you where you do it, either directly within the struct or you add the definintion to `attributes.inl`.

3. Add the meta data type to the `types` tuple at the end of `attributes.h`.

That's it. You can now choose to add your meta data to your tensor field in the `Tensor Field 3D Meta Data` processor and use it in subsequent import/export, plotting, and the like.