
Vue.component('entity-table', {
  props: [ "data" ],
  methods: {
    component_headers: function(name) {
      const value = this.data.data[name][0];
      let headers = [];
      for (key in value) {
        headers.push(key);
      }
      return headers;
    },
    component_member: function(value) {
      if (Array.isArray(value)) {
        return "[...]";
      } else if (typeof value === 'object') {
        return "{...}";
      } else if (typeof value === 'number') {
        return value.toFixed(1);
      } else {
        return value;
      }
    },
    type: function(value) {
      if (Array.isArray(value)) {
        return 'array';
      } else if (typeof value === 'object') {
        return 'object';
      } else if (typeof value === 'number') {
        return 'number';
      } else if (typeof value === 'boolean') {
        return 'boolean';
      } else if (typeof value === 'string') {
        return 'string';
      } else {
        return '';
      }
    },
    value_css: function(value) {
      return "property-value-" + this.type(value);
    }
  },
  template: `
    <div class="entity-table">
      <div class="entity-table-body">
        <table>
          <thead>
            <tr>
              <th class="table-component">Id</th>
              <th class="table-separator"></th>
              <th class="table-component">Name</th>
              <template v-for="(component,key) in data.data" v-if="key != 'Name'">
                <th class="table-separator"></th>
                <th class="table-component" :colspan="component_headers(key).length"> {{ key }} </th>
              </template>
            </tr>
            <tr>
              <th class="table-member"></th>
              <th class="table-separator"></th>
              <th class="table-member"></th>
              <template v-for="(component, name) in data.data" v-if="name != 'Name'">
                <th class="table-separator"></th>
                <th class="table-member" v-for="member in component_headers(name)">{{ member }}</th>
              </template>
            </tr>
          </thead>
          <tbody>
            <tr v-for="(entity, i) in data.entities">
              <td class="table-value">{{ entity }}</td>
              <td class="table-separator"></td>
              <td class="table-value">{{ data.data.Name[i].value }}</td>
              <template v-for="(component,key) in data.data" v-if="key != 'Name'">
                <td class="table-separator"></td>
                <td class="table-value" v-for="member in component_headers(key)" :class="value_css(component[i][member])">
                  {{ component_member(component[i][member]) }}
                </td>
              </template>               
            </tr>
          </tbody>
        </table>
      </div>
    </div>
    `
  });
  