<template id="roots">
  <el-table :data="showTable" style="width: 100%">

    <el-table-column label="ID" >
      <template #default="scope">
        <div style="display: flex; align-items: center">
          <span style="margin-left: 10px">{{ scope.row.id }}</span>
        </div>
      </template>
    </el-table-column>
    <el-table-column label="上传时间" >
      <template #default="scope">
        <div style="display: flex; align-items: center">
          <el-icon><timer /></el-icon>  
          <span style="margin-left: 10px">{{ scope.row.date }}</span>
        </div>
      </template>
    </el-table-column>
    <el-table-column label="文件名称">
      <template #default="scope">
        <div>{{ scope.row.filename }}</div>
      </template>
    </el-table-column>
    <el-table-column label="操作">
      <template #default="scope">
        <el-button size="small" @click="handleEdit(scope.$index, scope.row)"
          >下载</el-button
        >
      </template>
    </el-table-column>
  </el-table>
  <div style="margin: 10px 0;float: right;">
    <el-pagination @current-change = "changePages" :page-count="sumPage" background layout="prev, pager, next" :total="1000" />
  </div>
</template>

<script lang="ts" setup>
import { ref, computed ,reactive} from 'vue'
import { Timer } from '@element-plus/icons-vue'
import axios from "axios"
const tableData = reactive([])
let currentPage = ref(1);
let sumPage = ref(1);
const fetchData =async ()=>{
  if(tableData.length % 10 !=0){
    console.log(tableData.length)
      return 0;
  }
  let arrlen = tableData.length / 10 + 1
  let res = await axios({
  method: 'get',
  url: '/api/getData',
  params:{
    "page":arrlen
  }
})
tableData.push(...(res.data.data))
sumPage.value = Math.ceil(res.data.size / 10)

}
fetchData()
const handleEdit =async (index, row) => {
  let res = await axios({
  method: 'get',
  url: '/api/download',
  params:{
    "id":row.id
  }
})
window.open(`/static/${res.data.data.path}?filename=${res.data.data.filename}`)
}
const changePages = (e)=>{
  fetchData()
  currentPage.value = e
}
const showTable = computed(() => {
    return  tableData.slice(10*(currentPage.value-1),10*(currentPage.value-1)+10)
})

</script>
<style>
#roots{
  width: 100%;
  height: 100vh;
}
#app{
  width: 100%;
  height: 100vh;
  padding: 0;
  margin: 0 auto;
}
</style>