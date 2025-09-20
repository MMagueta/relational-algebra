/**
 * @mainpage Relational Algebra Engine
 *
 * [[file:./docs/images/elements.png]]
 *
 * The <b>Relational Algebra Engine</b> implements relational algebra in its
 * strict, theoretical form, adhering rigorously to the principles of
 * relational theory as articulated by E. F. Codd.
 *
 * A <b>relation</b> is modeled as a set of tuples, each tuple being a set
 * of attributes. Each attribute is a pairing of a name with a value
 * drawn from a domain. All operations are defined on <b>entire relations
 * as sets</b>, with no recourse to the compromises typical of SQL or other
 * practical implementations.
 *
 * The engine is intended as a <b>study in correctness</b>: each operation,
 * from projection to join, is executed in a manner consistent with the
 * classical definition of relational algebra. It is a platform for
 * exploring the consequences of relational theory in a low-level
 * programming context, providing both a tool for experimentation and a
 * concrete realization of the abstract model.
 *
 * @section structure Project Structure
 * - <code>src/</code> — C source files implementing the engine
 * - <code>include/</code> — Header files for public APIs
 * - <code>docs/</code> — Documentation and images
 * - <code>Makefile</code> — Build and formatting commands
 * - <code>Doxyfile</code> — Doxygen configuration for code documentation
 *
 * @section documentation Documentation
 *
 * <a href="https://MMagueta.github.io/relational-algebra/">View the latest API docs</a>
 *
 * To build the documentation locally:
 * @code{.sh}
 * doxygen Doxyfile
 * @endcode
 *
 * The HTML output will be in <code>docs/doxygen/html/</code>. GitHub Actions will auto-deploy docs from the <code>docs/doxygen/html</code> directory to the <code>gh-pages</code> branch.
 *
 * @section usage Usage
 *
 * Compile and run the engine:
 * @code{.sh}
 * make
 * @endcode
 *
 * Format the source code:
 * @code{.sh}
 * make format
 * @endcode
 *
 * @section contributing Contributing
 * Pull requests and issues are welcome! Please format code with clang-format before submitting.
 *
 * @section license License
 * This project is licensed under the APACHE 2.0 License. See the LICENSE file for details.
 */
