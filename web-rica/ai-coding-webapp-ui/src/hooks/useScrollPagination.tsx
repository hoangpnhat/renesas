import { useState } from "react"

export const useScrollPagination = () => {
  const [page, setPage] = useState(0)
  const [totalPage, setTotalPage] = useState(0)

  return {page, totalPage, setPage, setTotalPage}
}