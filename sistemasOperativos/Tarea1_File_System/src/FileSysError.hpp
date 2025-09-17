/// @copyright Los Más Fritos - 2025

#include <stdexcept>
#include <string>

/// Lista de posibles errores
enum errors {
  ERR_OCCUPIED_FILENAME = EXIT_FAILURE,
  ERR_OCCUPIED_BLOCK,
  ERR_OCCUPIED_INODE,
  ERR_NO_FREE_BLOCKS,
  ERR_NO_FREE_INODES,
  ERR_EMPTY_BLOCK,
  ERR_EMPTY_INODE,
  ERR_NO_INDEX_FOUND,
  ERR_NO_FILE_FOUND,
  ERR_FILE_ALREADY_OPEN,
  ERR_FILE_ALREADY_CLOSE,
  ERR_MEMORY_FULL,
  ERR_OUT_OF_RANGE
};

/// @brief Clase para manejo de errores locales
class FileSysError : public std::runtime_error {
 private:
  /// Código del error
  errors err_code;

 public:
  explicit FileSysError(errors code, const std::string& message)
      : std::runtime_error(message), err_code(code) {
  }
  /// retorna el código del error asignado
  errors code() const {
    return err_code;
  }

};
